#pragma once
// TODO : finish from version 1.0 desc and add sampler support
#include "macros.hpp"
#include "hakupch.hpp"
#include "HakuLog.hpp"
#include "directx/d3d12.h"

namespace Haku
{
namespace Renderer
{
enum
{
	ROOT_PARAMETER_NONE = 0,
	ROOT_PARAMETER_INLINE_CONST,
	ROOT_PARAMETER_INLINE_SRV,
	ROOT_PARAMETER_INLINE_CBV,
	ROOT_PARAMETER_INLINE_UAV,
	ROOT_PARAMETER_INLINE_SRV1,
	ROOT_PARAMETER_INLINE_CBV1,
	ROOT_PARAMETER_INLINE_UAV1,
	ROOT_PARAMETER_TABLE_UAV,
	ROOT_PARAMETER_TABLE_CBV,
	ROOT_PARAMETER_TABLE_SRV
};
class RootSigLayout
{
	// https://docs.microsoft.com/en-us/windows/win32/direct3d12/root-signature-limits#memory-limits-and-costs
	const uint8_t MAX_SIZE = 64u; // maximum of all the parameters in a root signature
								  /// There are three types of parameters that can be bound to a parameter UAV,CBV,SRV
	/// based on the type of binding that is done it may take either one or two DWORDS of space from the total 64 words
public:
	RootSigLayout();
	RootSigLayout(const RootSigLayout& rhs);
	RootSigLayout(const RootSigLayout&& rhs);
	RootSigLayout& operator=(const RootSigLayout& rhs);
	RootSigLayout& operator=(const RootSigLayout&& rhs);

	void AddRootConstant(uint8_t shaderreg, uint8_t registerspace);
	void AddRootSRV(uint8_t shaderreg, uint8_t registerspace);
	void AddRootUAV(uint8_t shaderreg, uint8_t registerspace);
	void AddRootCBV(uint8_t shaderreg, uint8_t registerspace);
	void AddDescriptorTableSRV(uint8_t shaderreg, uint8_t registerspace, uint8_t descriptorcount);
	void AddDescriptorTableUAV(uint8_t shaderreg, uint8_t registerspace, uint8_t descriptorcount);
	void AddDescriptorTableCBV(uint8_t shaderreg, uint8_t registerspace, uint8_t descriptorcount);
	D3D12_VERSIONED_ROOT_SIGNATURE_DESC GenerateVersionedRootSignature(
		D3D_ROOT_SIGNATURE_VERSION version,
		uint32_t				   flags = D3D12_ROOT_SIGNATURE_FLAG_ALLOW_INPUT_ASSEMBLER_INPUT_LAYOUT);

public:
	struct parameter_prop
	{
		uint8_t	 regspace;
		uint8_t	 shaderreg;
		uint32_t descriptorcount;
	};

private:
	D3D12_VERSIONED_ROOT_SIGNATURE_DESC GenerateVersionedRootSignature_1_0(uint32_t flags);
	D3D12_VERSIONED_ROOT_SIGNATURE_DESC GenerateVersionedRootSignature_1_1(uint32_t flags);

private:
	std::array<parameter_prop, MAX_ROOT_PARAMETER_COUNT> m_TablePropArray;
	std::array<uint8_t, MAX_ROOT_PARAMETER_COUNT>		 m_ParameterTracker;
	uint8_t												 m_Position		  = 0;
	uint8_t												 m_ParameterCount = 0;
};
class RootSignature
{
public:
	RootSignature(const RootSigLayout& layout);

	ID3D12RootSignature* Get();
	void				 Release() noexcept;

private:
	RootSigLayout						m_Layout;
	ID3D12RootSignature*				m_Signature;
	D3D12_VERSIONED_ROOT_SIGNATURE_DESC m_Description;
};
} // namespace Renderer
} // namespace Haku
