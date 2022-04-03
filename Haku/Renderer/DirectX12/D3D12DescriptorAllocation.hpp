#pragma once

#include "macros.hpp"
#include "hakupch.hpp"
#include "directx/d3d12.h"
#include "D3D12DescriptorPage.hpp"

namespace Haku
{
namespace Renderer
{
class DescriptorAllocation
{
public:
	DescriptorAllocation();
	DescriptorAllocation(
		std::shared_ptr<DescriptorPage> owner,
		D3D12_CPU_DESCRIPTOR_HANDLE		base_ptr,
		uint64_t						base_offet,
		uint64_t						size);
	~DescriptorAllocation();

	const uint64_t size() noexcept;
	const bool	   Is_Null() noexcept;
	const uint64_t GetOffset() noexcept;

private:
	uint64_t						m_Size;
	uint64_t						m_BaseOffset;
	D3D12_CPU_DESCRIPTOR_HANDLE		m_BaseHandle;
	std::shared_ptr<DescriptorPage> m_OwningPage;
};
} // namespace Renderer
} // namespace Haku
