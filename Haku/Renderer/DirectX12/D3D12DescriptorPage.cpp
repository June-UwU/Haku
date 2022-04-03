#include "HakuLog.hpp"
#include "D3D12Renderer.hpp"
#include "D3D12DescriptorPage.hpp"
#include "D3D12DescriptorAllocation.hpp"

namespace Haku
{
namespace Renderer
{
DescriptorPage::DescriptorPage(D3D12_DESCRIPTOR_HEAP_TYPE type, uint64_t size)
	: m_FreeSize(size)
{
	auto Device			= RenderEngine::GetDeviceD3D();
	m_HeapIncrementSize = Device->GetDescriptorHandleIncrementSize(type);
	// this is a cpu only heap allocator for staging resources in cpu
	D3D12_DESCRIPTOR_HEAP_DESC heap_properties{};
	heap_properties.NodeMask	   = 0;
	heap_properties.Type		   = type;
	heap_properties.NumDescriptors = size;
	heap_properties.Flags		   = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	// creating the heap for page
	Device->CreateDescriptorHeap(&heap_properties, IID_PPV_ARGS(&m_Heap));

	// Allocation is done a big page and is suballocated as nessary
	m_BasePtr = m_Heap->GetCPUDescriptorHandleForHeapStart();

	HAKU_LOG_CRIT("\n\tCPU PAGE CREATION \n Size : ", size, "\t Base : ", m_BasePtr.ptr, "\n");

	m_OffsetHashMap.emplace(m_BasePtr.ptr, size);
	m_SizeMap.emplace(m_BasePtr.ptr, m_OffsetHashMap.find(m_BasePtr.ptr));
}
DescriptorPage::~DescriptorPage()
{
	m_Heap->Release();
	m_Heap = nullptr;
}
const uint64_t DescriptorPage::size() const noexcept
{
	return m_FreeSize;
}
void DescriptorPage::ReleaseStaleAllocations() noexcept
{
	// TODO : finish this function logic
	// this function recovers all the blocks that are stale and yet no free
	while (!m_StaleQueue.empty())
	{
		auto front = m_StaleQueue.front();

		Release(front);

		m_StaleQueue.pop();
	}
}
DescriptorAllocation DescriptorPage::Allocate(uint64_t allocationsize) noexcept
{
	// TODO : finish the allocation logic
	auto block_list = m_SizeMap.lower_bound(allocationsize);

	if (block_list == m_SizeMap.end())
	{
		HAKU_LOG_WARN("No free block of size : ", allocationsize, "was found on page");
		return DescriptorAllocation();
	}

	auto size	   = block_list->first;
	auto offset_it = block_list->second;
	auto offset	   = offset_it->first;
	// creating new allocation for ret_val
	auto ret_allocation = DescriptorAllocation(this->shared_from_this(), m_BasePtr, offset, allocationsize);

	// removing the previous block
	m_OffsetHashMap.erase(offset_it);

	// adding new entry
	m_OffsetHashMap.emplace(offset + allocationsize, size - allocationsize);
	m_SizeMap.emplace(size - allocationsize, m_OffsetHashMap.find(offset + allocationsize));

	return ret_allocation;
}
void DescriptorPage::Free(DescriptorAllocation& allocation) noexcept
{
	auto offset = allocation.GetOffset();
	auto size	= allocation.size();
	HAKU_LOG_WARN("\n\tCPU RESOURCE STALE  \n Size : ", size, "\t Base", offset, "\n");
	StaleAllocation val;
	val.size   = allocation.size();
	val.Offset = allocation.GetOffset();

	m_StaleQueue.push(val);
}
void DescriptorPage::Release(StaleAllocation allocation) noexcept
{
	auto next_iter = m_OffsetHashMap.upper_bound(allocation.Offset);

	auto prev_iter = next_iter;

	if (prev_iter != m_OffsetHashMap.begin())
	{
		--prev_iter;
	}
	else
	{
		prev_iter = m_OffsetHashMap.end();
	}

	if (prev_iter != m_OffsetHashMap.end() && allocation.Offset == prev_iter->first + prev_iter->second)
	{
		allocation.Offset = prev_iter->first;
		allocation.size += prev_iter->second;

		m_SizeMap.erase(prev_iter->second);
		m_OffsetHashMap.erase(prev_iter);
	}

	if (next_iter != m_OffsetHashMap.end() && next_iter->first == allocation.Offset + allocation.size)
	{
		allocation.size += next_iter->second;

		m_SizeMap.erase(next_iter->second);
		m_OffsetHashMap.erase(next_iter);
	}

	AddNewBlock(allocation.Offset, allocation.size);
}
void DescriptorPage::AddNewBlock(uint64_t offset, uint64_t size) noexcept
{
	m_OffsetHashMap.emplace(offset, size);
	m_SizeMap.emplace(size, m_OffsetHashMap.find(offset));
}
} // namespace Renderer
} // namespace Haku
