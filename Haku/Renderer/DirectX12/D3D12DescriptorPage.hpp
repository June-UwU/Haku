#pragma once

#include "macros.hpp"
#include "hakupch.hpp"
#include <directx/d3d12.h>

// TODO :
// funtions :
// ReleaseStaleAllocations()
// Allocate()

// Size is the number of descriptor in heap / allocations done
namespace Haku
{
namespace Renderer
{
class DescriptorAllocation;

class DescriptorPage : std::enable_shared_from_this<DescriptorPage>
{
	// A simple block that holds all the block information

	struct StaleAllocation
	{
		uint64_t size;
		uint64_t Offset;
	};

	using Size		 = uint64_t;
	using Offset	 = uint64_t;
	using StaleQueue = std::queue<StaleAllocation>;
	using OffSetMap	 = std::unordered_map<Offset, Size>;
	using SizeMap	 = std::unordered_multimap<Size, OffSetMap::iterator>;

public:
	// size is the descriptor count that is available
	DescriptorPage(D3D12_DESCRIPTOR_HEAP_TYPE type, uint64_t size = 1024);
	~DescriptorPage();

	const uint64_t		 size() const noexcept;
	void				 ReleaseStaleAllocations() noexcept;
	DescriptorAllocation Allocate(uint64_t allocationsize) noexcept;
	void				 Free(DescriptorAllocation& allocation) noexcept;

private:
	void Release(StaleAllocation allocation) noexcept;
	void AddNewBlock(uint64_t offset, uint64_t size) noexcept;

private:
	ID3D12DescriptorHeap*		m_Heap;
	D3D12_CPU_DESCRIPTOR_HANDLE m_BasePtr;
	SizeMap						m_SizeMap;
	Size						m_FreeSize;
	StaleQueue					m_StaleQueue;
	OffSetMap					m_OffsetHashMap;
	uint64_t					m_HeapIncrementSize;
};
} // namespace Renderer
} // namespace Haku
