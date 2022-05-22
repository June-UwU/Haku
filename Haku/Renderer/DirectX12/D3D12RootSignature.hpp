#pragma once

#include "wil/wrl.h"
#include "macros.hpp"
#include "hakupch.hpp"
#include "HakuLog.hpp"

#include "D3D12RenderDevice.hpp"
#include "D3D12CommandQueue.hpp"

// void AddDescriptorTable(D3D12DescriptorTable& table) noexcept;
// intentional design as we don't hold local descriptor table and tables are expected to be alive till creation  :

namespace Haku
{
namespace Renderer
{
// this class doesn't support Root Descriptor since the cost is not worth the complexity attached
class RootSignature
{
	//
	using ParameterBitMask = uint64_t;
	// maximum no of parameter
	constexpr static const uint32_t Root_Signature_Limit = MAX_ROOT_PARAMETER_COUNT;

public:
	RootSignature(const D3D12_ROOT_SIGNATURE_DESC& desc, D3D_ROOT_SIGNATURE_VERSION version);
	DISABLE_COPY(RootSignature)
	DISABLE_MOVE(RootSignature)
	~RootSignature();

	void	 Destroy();
	uint64_t GetDescriptorCount(uint64_t pos) noexcept;
	D3D12_ROOT_SIGNATURE_DESC GetRootDescriptor() noexcept;
	uint64_t GetBitMask(D3D12_DESCRIPTOR_HEAP_TYPE type) noexcept;
	void	 Initialize(const D3D12_ROOT_SIGNATURE_DESC& desc, D3D_ROOT_SIGNATURE_VERSION version);
private:
	ID3D12RootSignature*					   m_Signature;
	D3D12_ROOT_SIGNATURE_DESC				   m_Descriptor;
	std::array<uint32_t, Root_Signature_Limit> m_DescriptorCount;

	ParameterBitMask m_Sampler;
	ParameterBitMask m_DescriptorTable;
};

} // namespace Renderer
} // namespace Haku
