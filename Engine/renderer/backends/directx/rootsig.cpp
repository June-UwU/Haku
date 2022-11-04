/*****************************************************************//**
 * \file   rootsig.cpp
 * \brief  
 * 
 * \author June
 * \date   November 2022
 *********************************************************************/
#include "rootsig.hpp"
#include "core/logger.hpp"
#include "directx_backend.hpp"

ID3D12RootSignature* create_rootsignature(u32 number_of_rparam, D3D12_ROOT_PARAMETER* param, u32 number_of_sampler, D3D12_STATIC_SAMPLER_DESC* samplers, D3D12_ROOT_SIGNATURE_FLAGS flags)
{
	ID3DBlob* err_blob = nullptr;
	ID3DBlob* version_sig = nullptr;
	directx_device* dev = get_device();
	ID3D12RootSignature* rv = nullptr;

	D3D12_VERSIONED_ROOT_SIGNATURE_DESC version_desc{};

	version_desc.Version = D3D_ROOT_SIGNATURE_VERSION_1_0;

	D3D12_ROOT_SIGNATURE_DESC rt_desc{};
	rt_desc.Flags = flags;
	rt_desc.NumParameters = number_of_rparam;
	rt_desc.NumStaticSamplers = number_of_sampler;
	rt_desc.pParameters = param;
	rt_desc.pStaticSamplers = samplers;

	version_desc.Desc_1_0 = rt_desc;

	HRESULT  dx_rv = D3D12SerializeVersionedRootSignature(&version_desc, &version_sig, &err_blob);
	if (S_OK != dx_rv)
	{
		HLEMER("failed to serialize root signature %s", (char*)err_blob->GetBufferPointer());
		err_blob->Release();
		return nullptr;
	}

	dx_rv = dev->logical_device->CreateRootSignature(0, version_sig->GetBufferPointer(), version_sig->GetBufferSize(), IID_PPV_ARGS(&rv));
	if (S_OK != dx_rv)
	{
		HLEMER("failed to create directx root signature");
		version_sig->Release();
		return nullptr;
	}

	version_sig->Release();

	return rv;
}

void destroy_rootsignature(ID3D12RootSignature* signature)
{
	signature->Release();
}
