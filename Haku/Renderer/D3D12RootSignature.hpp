#include "wil/wrl.h"
#include "../macros.hpp"
#include "../hakupch.hpp"
#include "D3D12RenderDevice.hpp"
#include "D3D12CommandQueue.hpp"

// TODO : ADD SAMPLER SUPPORT
/// samplers are missing from the descriptor table and desc structure and I can't be bothered to add it now
/*Root sigs  stay the same the entire runtime and depending on the shaders and psos root sigs need to be bound*/
/*Do I add this as part of a pso..? would that make sense..?*/
namespace Haku
{
namespace Renderer
{
class D3D12DescriptorTable
{
public:
	D3D12DescriptorTable(uint32_t range_size = 1) noexcept;
	template<uint32_t basereg, uint32_t regspace>
	void AddSRV(
		uint32_t					 NumberDescriptor,
		D3D12_DESCRIPTOR_RANGE_FLAGS flags,
		UINT						 offset = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND) noexcept
	{
		AddRange<basereg, regspace>(D3D12_DESCRIPTOR_RANGE_TYPE_SRV, NumberDescriptor, flags, offset);
		return *this;
	}

	template<uint32_t basereg, uint32_t regspace>
	D3D12DescriptorTable& AddUAV(
		uint32_t					 NumberDescriptor,
		D3D12_DESCRIPTOR_RANGE_FLAGS flags,
		UINT						 offset = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND) noexcept
	{
		AddRange<basereg, regspace>(D3D12_DESCRIPTOR_RANGE_TYPE_UAV, NumberDescriptor, flags, offset);
		return *this;
	}

	template<uint32_t basereg, uint32_t regspace>
	D3D12DescriptorTable& AddCBV(
		uint32_t					 NumberDescriptor,
		D3D12_DESCRIPTOR_RANGE_FLAGS flags,
		UINT						 offset = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND) noexcept
	{
		AddRange<basereg, regspace>(D3D12_DESCRIPTOR_RANGE_TYPE_CBV, NumberDescriptor, flags, offset);
		return *this;
	}

	template<uint32_t basereg, uint32_t regspace>
	D3D12DescriptorTable& AddSampler(
		uint32_t					 NumberDescriptor,
		D3D12_DESCRIPTOR_RANGE_FLAGS flags,
		UINT						 offset = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND) noexcept
	{
		AddRange<basereg, regspace>(D3D12_DESCRIPTOR_RANGE_TYPE_SAMPLER, NumberDescriptor, flags, offset);
		return *this;
	}

	UINT					 size() noexcept { return static_cast<UINT>(m_Table.size()); }
	D3D12_DESCRIPTOR_RANGE1* data() noexcept { return m_Table.data(); }

private:
	template<uint32_t basereg, uint32_t regspace>
	D3D12DescriptorTable& AddRange(
		D3D12_DESCRIPTOR_RANGE_TYPE	 type,
		uint32_t					 NumberDescriptor,
		D3D12_DESCRIPTOR_RANGE_FLAGS flags,
		UINT						 offset = D3D12_DESCRIPTOR_RANGE_OFFSET_APPEND) noexcept
	{
		D3D12_DESCRIPTOR_RANGE1 range{};
		range.RangeType							= type;
		range.BaseShaderRegister				= basereg;
		range.Flags								= flags;
		range.OffsetInDescriptorsFromTableStart = offset;
		range.RegisterSpace						= regspace;
		range.NumDescriptors					= NumberDescriptor;
		m_Table.push_back(range);
		return *this;
	}

private:
	// use smart structures...?
	std::vector<D3D12_DESCRIPTOR_RANGE1> m_Table;
};

class D3D12RootSignatureDesc
{
public:
	D3D12RootSignatureDesc() = default;

	void AddDescriptorTable(D3D12DescriptorTable& table) noexcept;
	template<uint32_t basereg, uint32_t regspace>
	D3D12RootSignatureDesc& Add32BitConstant(const uint32_t number_of_values) noexcept
	{
		D3D12_ROOT_PARAMETER1 parameter{};
		parameter.Constants.RegisterSpace  = regspace;
		parameter.Constants.ShaderRegister = basereg;
		parameter.Constants.Num32BitValues = number_of_values;
		parameter.ParameterType			   = D3D12_ROOT_PARAMETER_TYPE_32BIT_CONSTANTS;
		AddParameter(parameter);
		return *this;
	}

	template<uint32_t basereg, uint32_t regspace>
	D3D12RootSignatureDesc& AddCBV(D3D12_ROOT_DESCRIPTOR_FLAGS flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE) noexcept
	{
		D3D12_ROOT_PARAMETER1 parameter{};
		parameter.Descriptor.Flags			= flags;
		parameter.Descriptor.ShaderRegister = basereg;
		parameter.Descriptor.RegisterSpace	= regspace;
		parameter.ParameterType				= D3D12_ROOT_PARAMETER_TYPE_CBV;
		AddParameter(parameter);
		return *this;
	}

	template<uint32_t basereg, uint32_t regspace>
	D3D12RootSignatureDesc& AddUAV(D3D12_ROOT_DESCRIPTOR_FLAGS flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE) noexcept
	{
		D3D12_ROOT_PARAMETER1 parameter{};
		parameter.Descriptor.Flags			= flags;
		parameter.Descriptor.ShaderRegister = basereg;
		parameter.Descriptor.RegisterSpace	= regspace;
		parameter.ParameterType				= D3D12_ROOT_PARAMETER_TYPE_UAV;
		AddParameter(parameter);
		return *this;
	}

	template<uint32_t basereg, uint32_t regspace>
	D3D12RootSignatureDesc& AddSRV(D3D12_ROOT_DESCRIPTOR_FLAGS flags = D3D12_ROOT_DESCRIPTOR_FLAG_NONE) noexcept
	{
		D3D12_ROOT_PARAMETER1 parameter{};
		parameter.Descriptor.Flags			= flags;
		parameter.Descriptor.ShaderRegister = basereg;
		parameter.Descriptor.RegisterSpace	= regspace;
		parameter.ParameterType				= D3D12_ROOT_PARAMETER_TYPE_SRV;
		AddParameter(parameter);
		return *this;
	}

	D3D12_VERSIONED_ROOT_SIGNATURE_DESC Build() noexcept;

	void LocalRootSig() noexcept;
	void DenyMeshShader() noexcept;
	void DenyHullShader() noexcept;
	void DenyPixelShader() noexcept;
	void AllowInputLayout() noexcept;
	void DenyVertexShader() noexcept;
	void DenyDomainShader() noexcept;
	void AllowStreamOutput() noexcept;
	void DenyGeometryShader() noexcept;
	void DirectIndexSamplerHeap() noexcept;
	void DirectIndexCBV_SRV_UAV() noexcept;
	void DenyAmplicificationShader() noexcept;

private:
	void							   AddParameter(D3D12_ROOT_PARAMETER1& Parameter);
	std::vector<D3D12_ROOT_PARAMETER1> m_Parameter;
	D3D12_ROOT_SIGNATURE_FLAGS		   m_Flags = D3D12_ROOT_SIGNATURE_FLAG_NONE;
};

class D3D12RootSignature
{
public:
	D3D12RootSignature(D3D12RootSignatureDesc& Desc, D3D12RenderDevice& Device);
	ID3D12RootSignature* Get() noexcept { return m_Signature.Get(); }
	void				 SetSignature(D3D12CommandQueue& Command) noexcept;

private:
	Microsoft::WRL::ComPtr<ID3D12RootSignature> m_Signature;
};

} // namespace Renderer
} // namespace Haku
