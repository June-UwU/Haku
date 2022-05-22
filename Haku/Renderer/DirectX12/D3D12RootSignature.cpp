#include "directx/d3dx12.h"
#include "Core/Exceptions.hpp"
#include "D3D12RootSignature.hpp"
#include "Renderer/DirectX12/D3D12Renderer.hpp"

// PRESENT : redacting texture binding for testing descriptor table

namespace Haku
{
namespace Renderer
{

RootSignature::RootSignature(const D3D12_ROOT_SIGNATURE_DESC& desc, D3D_ROOT_SIGNATURE_VERSION version)
	: m_Sampler(0)
	, m_DescriptorTable(0)
{
	Initialize(desc, version);
}
RootSignature::~RootSignature()
{
	Destroy();
	m_Signature->Release();
}
void RootSignature::Destroy()
{
	for (uint32_t i = 0; i < m_Descriptor.NumParameters; i++)
	{
		const auto& RootParameter = m_Descriptor.pParameters[i];
		if (RootParameter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
		{
			delete[] RootParameter.DescriptorTable.pDescriptorRanges;
		}
	}

	// deleting the root parameters
	delete[] m_Descriptor.pParameters;
	m_Descriptor.NumParameters = 0;
	m_Descriptor.pParameters   = nullptr;

	// deleting the static params
	delete[] m_Descriptor.pStaticSamplers;
	m_Descriptor.NumStaticSamplers = 0;
	m_Descriptor.pStaticSamplers   = nullptr;

	// resetting the bit masks
	m_DescriptorTable = 0;
	m_Sampler		  = 0;
}

uint64_t RootSignature::GetDescriptorCount(uint64_t pos) noexcept
{
	return m_DescriptorCount[pos];
}

D3D12_ROOT_SIGNATURE_DESC RootSignature::GetRootDescriptor() noexcept
{
	return m_Descriptor;
}

uint64_t RootSignature::GetBitMask(D3D12_DESCRIPTOR_HEAP_TYPE type) noexcept
{
	uint64_t retval{ 0 };
	switch (type)
	{
	case D3D12_DESCRIPTOR_HEAP_TYPE_SAMPLER:
		retval = m_Sampler;
		break;
	case D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV:
		retval = m_DescriptorTable;
		break;
	}
	return retval;
}

void RootSignature::Initialize(const D3D12_ROOT_SIGNATURE_DESC& desc, D3D_ROOT_SIGNATURE_VERSION version)
{
	Destroy();
	auto	   Device			   = RenderEngine::GetDeviceD3D();
	const auto NumberOfDescriptors = desc.NumParameters;

	D3D12_ROOT_PARAMETER* ParameterPtr =
		NumberOfDescriptors > 0 ? new D3D12_ROOT_PARAMETER[NumberOfDescriptors] : nullptr;

	for (uint32_t i = 0; i < NumberOfDescriptors; i++)
	{
		const D3D12_ROOT_PARAMETER& ParameterRefIter = desc.pParameters[i];
		ParameterPtr[i]								 = ParameterRefIter;

		if (ParameterRefIter.ParameterType == D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE)
		{
			auto NumberOfRanges = ParameterRefIter.DescriptorTable.NumDescriptorRanges;
			// if there is any range make space for them
			D3D12_DESCRIPTOR_RANGE* DescriptorRangePtr =
				NumberOfRanges ? new D3D12_DESCRIPTOR_RANGE[NumberOfRanges] : nullptr;

			// copy the parameter to our ptr
			memcpy(
				DescriptorRangePtr,
				ParameterRefIter.DescriptorTable.pDescriptorRanges,
				(sizeof(D3D12_DESCRIPTOR_RANGE) * NumberOfRanges));

			ParameterPtr[i].DescriptorTable.NumDescriptorRanges = NumberOfRanges;
			ParameterPtr[i].DescriptorTable.pDescriptorRanges	= DescriptorRangePtr;

			// if there are ranges check for the type and set the approiate bit mask
			if (NumberOfRanges)
			{
				switch (ParameterRefIter.ParameterType)
				{
				case D3D12_DESCRIPTOR_RANGE_TYPE_CBV:
				case D3D12_DESCRIPTOR_RANGE_TYPE_SRV:
				case D3D12_DESCRIPTOR_RANGE_TYPE_UAV:
					m_DescriptorTable |= (1 << i);
					break;
				case D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER:
					m_Sampler |= (1 << i);
					break;
				}
			}

			for (uint32_t j = 0; i < NumberOfDescriptors; i++)
			{
				m_DescriptorCount[i] += DescriptorRangePtr[j].NumDescriptors;
			}
		}
	}

	m_Descriptor.NumParameters = NumberOfDescriptors;
	m_Descriptor.pParameters   = ParameterPtr;

	auto NumberOfStaticSampler = desc.NumStaticSamplers;

	D3D12_STATIC_SAMPLER_DESC* StaticSamplerPtr =
		NumberOfStaticSampler ? new D3D12_STATIC_SAMPLER_DESC[NumberOfStaticSampler] : nullptr;

	if (NumberOfStaticSampler)
	{
		memcpy(StaticSamplerPtr, desc.pStaticSamplers, (sizeof(D3D12_STATIC_SAMPLER_DESC) * NumberOfStaticSampler));
	}

	m_Descriptor.NumStaticSamplers = NumberOfStaticSampler;
	m_Descriptor.pStaticSamplers   = StaticSamplerPtr;

	auto Flags		   = desc.Flags;
	m_Descriptor.Flags = Flags;

	CD3DX12_VERSIONED_ROOT_SIGNATURE_DESC VersionedRootSigDesc;
	VersionedRootSigDesc.Init_1_0(NumberOfDescriptors, ParameterPtr, NumberOfStaticSampler, StaticSamplerPtr, Flags);

	Microsoft::WRL::ComPtr<ID3DBlob> Blob;
	Microsoft::WRL::ComPtr<ID3DBlob> ErrorBlob;

	HAKU_SOK_ASSERT_CHAR_PTR_ERROR_BLOB(
		D3DX12SerializeVersionedRootSignature(&VersionedRootSigDesc, version, &Blob, &ErrorBlob), ErrorBlob);

	HAKU_SOK_ASSERT(Device->CreateRootSignature(
		0,
		Blob->GetBufferPointer(),
		Blob->GetBufferSize(),
		__uuidof(ID3D12RootSignature),
		reinterpret_cast<void**>(&m_Signature)))
}
} // namespace Renderer
} // namespace Haku
