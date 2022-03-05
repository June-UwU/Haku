#include "D3D12RootSignature.hpp"
#include "../Core/Exceptions.hpp"

//PRESENT : redacting texture binding for testing descriptor table


namespace Haku
{
namespace Renderer
{
D3D12DescriptorTable::D3D12DescriptorTable(uint32_t range_size) noexcept
{
	m_Table.reserve(range_size);
}
void D3D12RootSignatureDesc::AddDescriptorTable(D3D12DescriptorTable& table) noexcept
{
	D3D12_ROOT_PARAMETER1 parameter{};
	parameter.DescriptorTable.NumDescriptorRanges = table.size();
	parameter.DescriptorTable.pDescriptorRanges	  = table.data();
	parameter.ParameterType						  = D3D12_ROOT_PARAMETER_TYPE_DESCRIPTOR_TABLE;
	m_Parameter.push_back(parameter);
}
D3D12_VERSIONED_ROOT_SIGNATURE_DESC D3D12RootSignatureDesc::Build() noexcept
{
	D3D12_VERSIONED_ROOT_SIGNATURE_DESC sigdesc{};
	sigdesc.Desc_1_1.Flags			   = m_Flags;
	sigdesc.Desc_1_1.NumStaticSamplers = m_Samplers.size();
	sigdesc.Desc_1_1.pStaticSamplers   = m_Samplers.data();
	sigdesc.Desc_1_1.NumParameters	   = m_Parameter.size();
	sigdesc.Desc_1_1.pParameters	   = m_Parameter.data();
	sigdesc.Version					   = D3D_ROOT_SIGNATURE_VERSION_1_1;

	return sigdesc;
}

void D3D12RootSignatureDesc::LocalRootSig() noexcept
{
	m_Flags |= D3D12_ROOT_SIGNATURE_FLAG_LOCAL_ROOT_SIGNATURE;
}
void D3D12RootSignatureDesc::DenyMeshShader() noexcept
{
	m_Flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_MESH_SHADER_ROOT_ACCESS;
}
void D3D12RootSignatureDesc::DenyHullShader() noexcept
{
	m_Flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_HULL_SHADER_ROOT_ACCESS;
}
void D3D12RootSignatureDesc::DenyPixelShader() noexcept
{
	m_Flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_PIXEL_SHADER_ROOT_ACCESS;
}
void D3D12RootSignatureDesc::AllowInputLayout() noexcept
{
	m_Flags |= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT;
}
void D3D12RootSignatureDesc::DenyVertexShader() noexcept
{
	m_Flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_VERTEX_SHADER_ROOT_ACCESS;
}
void D3D12RootSignatureDesc::DenyDomainShader() noexcept
{
	m_Flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_DOMAIN_SHADER_ROOT_ACCESS;
}
void D3D12RootSignatureDesc::AllowStreamOutput() noexcept
{
	m_Flags |= D3D12_ROOT_SIGNATURE_FLAG_ALLOW_STREAM_OUTPUT;
}
void D3D12RootSignatureDesc::DenyGeometryShader() noexcept
{
	m_Flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_GEOMETRY_SHADER_ROOT_ACCESS;
}
void D3D12RootSignatureDesc::DirectIndexSamplerHeap() noexcept
{
	m_Flags |= D3D12_ROOT_SIGNATURE_FLAG_SAMPLER_HEAP_DIRECTLY_INDEXED;
}
void D3D12RootSignatureDesc::DirectIndexCBV_SRV_UAV() noexcept
{
	m_Flags |= D3D12_ROOT_SIGNATURE_FLAG_CBV_SRV_UAV_HEAP_DIRECTLY_INDEXED;
}
void D3D12RootSignatureDesc::DenyAmplicificationShader() noexcept
{
	m_Flags |= D3D12_ROOT_SIGNATURE_FLAG_DENY_AMPLIFICATION_SHADER_ROOT_ACCESS;
}
void D3D12RootSignatureDesc::AddParameter(D3D12_ROOT_PARAMETER1& Parameter)
{
	m_Parameter.push_back(Parameter);
}
D3D12RootSignature::D3D12RootSignature(D3D12RootSignatureDesc& Desc, D3D12RenderDevice& Device)
{
	D3D12_FEATURE_DATA_ROOT_SIGNATURE featureData{};
	// This is the highest version the sample supports. If CheckFeatureSupport succeeds, the HighestVersion returned
	// will not be greater than this.
	featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_1;

	if (Device.get()->CheckFeatureSupport(D3D12_FEATURE_ROOT_SIGNATURE, &featureData, sizeof(featureData)) != S_OK)
	{
		featureData.HighestVersion = D3D_ROOT_SIGNATURE_VERSION_1_0;
	}
	auto							 sigdesc = Desc.Build();
	Microsoft::WRL::ComPtr<ID3DBlob> signature;
	Microsoft::WRL::ComPtr<ID3DBlob> error;
	HAKU_SOK_ASSERT_CHAR_PTR(
		D3DX12SerializeVersionedRootSignature(
			&sigdesc, featureData.HighestVersion, signature.GetAddressOf(), error.GetAddressOf()),
		error)
	HAKU_SOK_ASSERT(Device.get()->CreateRootSignature(
		0, signature->GetBufferPointer(), signature->GetBufferSize(), IID_PPV_ARGS(&m_Signature)));
	m_Signature->SetName(L"Root descriptor");
}
void D3D12RootSignature::SetSignature(D3D12CommandQueue& Command) noexcept
{
	Command.GetCommandList()->SetGraphicsRootSignature(m_Signature.Get());
}
} // namespace Renderer
} // namespace Haku
