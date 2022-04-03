#pragma once

#include <set>
#include "macros.hpp"
#include "hakupch.hpp"
#include <directx/d3d12.h>
#include "D3D12DescriptorPage.hpp"
#include "D3D12DescriptorAllocation.hpp"

// This is intended as a CPU descriptor allocator and is inspired from the free-list allocator of 3dgep and dilignent
// engine
// TODO : experiment with red-black trees
class DescriptorAllocation;

namespace Haku
{
namespace Renderer
{
class DescriptorAllocator
{
	using PagePool = std::vector<std::shared_ptr<DescriptorPage>>;

public:
	DescriptorAllocator(
		D3D12_DESCRIPTOR_HEAP_TYPE type			   = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV,
		uint32_t				   defaultpagesize = 1024);

	void				 ReleaseStaleAllocations() noexcept;
	DescriptorAllocation Allocate(uint32_t numberofpages) noexcept;

private:
	// helper function to make new page and insert into the free page vector for allocation
	void CreateNewPage() noexcept;

private:
	// held locally for page creation
	D3D12_DESCRIPTOR_HEAP_TYPE m_type;
	PagePool				   m_PagePool;
	uint32_t				   m_DefaultPageSize;
	std::set<uint64_t>		   m_FreePageIndexSet;
};
} // namespace Renderer
} // namespace Haku
