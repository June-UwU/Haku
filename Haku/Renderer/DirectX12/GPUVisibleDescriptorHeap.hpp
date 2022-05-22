#pragma once

#include <intrin.h>
#include "macros.hpp"
#include "hakupch.hpp"
#include "HakuLog.hpp"
#include "directx/d3d12.h"

// TODO : this might need improvement as the staging need to be somewhere off that can be done to
// handle multiple command list

/*GPU VISIBLE DESCRIPTOR HEAP*/
/*This is indended as a dynamically explanding descriptor heap that resizes itself as the need arrives
(check will happen before the copy,and ideally wouldn't happen in the runtime).The Descriptor Heap starts
with 1024 descriptors at first with 10% of the total heaps form the total descriptor heaps reserved as static
region and the rest divided into an even and odd bank that is split at 45% - 45% ratio .*/

/*GPU VISIBLE DESCRIPTOR HEAP*/

//		  static_region			even_region					odd_region
//			  (10%)				   (45%)					   (45%)
//		|--------------|---------------------------|---------------------------|

namespace Haku
{
namespace Renderer
{
class RootSignature;
class CommandList;

class GPUDescriptorHeap
{
public:
	GPUDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t number_of_descriptors = 1024u);
	DISABLE_COPY(GPUDescriptorHeap)
	DISABLE_MOVE(GPUDescriptorHeap)
	~GPUDescriptorHeap();

	void ParseRootSignature(RootSignature& rootsig) noexcept;
	void CommitStagedDescriptorForDraw(CommandList& commandlist);
	void CommitStagedDescriptorForDispatch(CommandList& commandlist);
	void StageDescriptors(uint32_t rootindex, uint32_t num_descriptor, const D3D12_CPU_DESCRIPTOR_HANDLE src);
	void CommitStagedDescriptor(
		ID3D12GraphicsCommandList*														   commandlist,
		std::function<void(ID3D12GraphicsCommandList*, UINT, D3D12_GPU_DESCRIPTOR_HANDLE)> setFunc) noexcept;

private:
	void	 CreateDescriptorHeap();
	void	 ResetCacheAndDescriptorHeap() noexcept;
	uint32_t ComputeStaleDescriptorCount() noexcept;

private:
	struct DescriptorCache
	{
		DescriptorCache()
			: ArrayOffset(0)
			, BaseHandle(nullptr)
			, NumberOfDescriptor(0u)
		{
		}
		D3D12_CPU_DESCRIPTOR_HANDLE* BaseHandle;
		uint32_t					 ArrayOffset;
		uint32_t					 NumberOfDescriptor;
	};

private:
	D3D12_DESCRIPTOR_HEAP_TYPE							  m_Type;
	ID3D12DescriptorHeap*								  m_VisibleHeap;
	D3D12_CPU_DESCRIPTOR_HANDLE							  m_CPUBaseHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE							  m_GPUBaseHandle;
	std::array<DescriptorCache, MAX_ROOT_PARAMETER_COUNT> m_DescriptorCache;
	uint32_t											  m_HeapIncrementSize;
	uint64_t											  m_DescriptorBitMask;
	uint64_t											  m_StaleDescriptorMask;
	D3D12_CPU_DESCRIPTOR_HANDLE*						  m_DescriptorHandleCache;
	uint32_t											  m_CurrentDescriptorHeapSize;

	/*Split Property*/
	/*General Property*/
	uint32_t m_StaticRegionSize;
	uint32_t m_DynamicRegionSize;

	/*Even Heap*/
	D3D12_CPU_DESCRIPTOR_HANDLE m_EvenCPUBaseHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE m_EvenGPUBaseHandle;

	/*Odd Heap*/
	D3D12_CPU_DESCRIPTOR_HANDLE m_OddCPUBaseHandle;
	D3D12_GPU_DESCRIPTOR_HANDLE m_OddGPUBaseHandle;
};
} // namespace Renderer
} // namespace Haku
