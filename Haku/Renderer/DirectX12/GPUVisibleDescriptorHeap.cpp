#include "directx/d3dx12.h"
#include "GPUVisibleDescriptorHeap.hpp"
#include "Renderer/DirectX12/D3D12Renderer.hpp"
#include "Renderer/DirectX12/D3D12RootSignature.hpp"
// this class is responsible for the management of GPU visible heaps and the commiting of CPU visible
// heaps to said heap for draw or dispatch calls

// TODO : Complete this

namespace Haku
{
namespace Renderer
{
GPUDescriptorHeap::GPUDescriptorHeap(D3D12_DESCRIPTOR_HEAP_TYPE type, uint32_t number_of_descriptors)
	: m_Type(type)
	, m_VisibleHeap(nullptr)
	, m_DescriptorBitMask(0)
	, m_StaleDescriptorMask(0)
	, m_DescriptorHandleCache(nullptr)
	, m_CurrentDescriptorHeapSize(number_of_descriptors)
{
	m_HeapIncrementSize = RenderEngine::GetDeviceD3D()->GetDescriptorHandleIncrementSize(type);
	CreateDescriptorHeap();
}
GPUDescriptorHeap::~GPUDescriptorHeap()
{
	//this checks for the nullptr of the owning resources and is safe
	ResetCacheAndDescriptorHeap();
}
void GPUDescriptorHeap::ParseRootSignature(RootSignature& rootsig) noexcept
{
	// TODO : This is not correct
	auto RootSigDesc	= rootsig.GetRootDescriptor();
	m_DescriptorBitMask = rootsig.GetBitMask(m_Type);

	auto	 DescriptorBitMask = m_DescriptorBitMask;
	uint32_t TotalDescriptorInRootSig{ 0 };
	DWORD	 index;
	while (_BitScanForward64(&index, DescriptorBitMask) && index < RootSigDesc.NumParameters)
	{
		auto NumDesc = rootsig.GetDescriptorCount(index);

		m_DescriptorCache[index].NumberOfDescriptor = NumDesc;
		m_DescriptorCache[index].ArrayOffset		= TotalDescriptorInRootSig;
		m_DescriptorCache[index].BaseHandle			= &m_DescriptorHandleCache[TotalDescriptorInRootSig];

		TotalDescriptorInRootSig += NumDesc;

		DescriptorBitMask ^= (1 << index);
	}

	if (TotalDescriptorInRootSig <= m_DynamicRegionSize)
	{
		HAKU_LOG_INFO("Haku Descriptor Dynamic Region Size Exceeded : ", m_DynamicRegionSize);
		// Increase Dynamic Region Size
		m_CurrentDescriptorHeapSize = 2 * m_CurrentDescriptorHeapSize;
		ResetCacheAndDescriptorHeap();
		CreateDescriptorHeap();
		HAKU_LOG_WARN("Retrying RootSignature Parse Dynamic Region Size : ", m_DynamicRegionSize);
		ParseRootSignature(rootsig); // ERROR_PRONE : logical error here can backfire in my face
	}
}
void GPUDescriptorHeap::CommitStagedDescriptorForDraw(CommandList& commandlist)
{
	// TODO : Set this up for the approiate functions
}
void GPUDescriptorHeap::CommitStagedDescriptorForDispatch(CommandList& commandlist)
{
	// TODO : set this to the approiate functions
}
void GPUDescriptorHeap::StageDescriptors(
	uint32_t						  rootindex,
	uint32_t						  num_descriptor,
	const D3D12_CPU_DESCRIPTOR_HANDLE src)
{
	if (num_descriptor > m_DynamicRegionSize)
	{
		HAKU_LOG_INFO("Haku Descriptor Dynamic Region Size Exceeded : ", m_DynamicRegionSize);
		// Increase Dynamic Region Size
		m_CurrentDescriptorHeapSize = 2 * m_CurrentDescriptorHeapSize;
		ResetCacheAndDescriptorHeap();
		CreateDescriptorHeap();
		HAKU_LOG_INFO(
			"Attempting An Increment (This might save this , but at this point this can still function normally but fail on copy or mid excute) HeapSize :",
			m_DynamicRegionSize);
	}

	if (rootindex >= MAX_ROOT_PARAMETER_COUNT)
	{
		HAKU_LOG_CRIT("Attempted A Resource Bind Outside the root index space \n ROOT INDEX : ", rootindex);
		throw std::bad_alloc();
	}
	/// This is risky as it gets as this will be depentant on the copydescriptor method..
	auto& TableCache = m_DescriptorCache[rootindex];
	if (num_descriptor > TableCache.NumberOfDescriptor)
	{
		HAKU_LOG_CRIT(
			"More Descriptors Than Expected : ", TableCache.NumberOfDescriptor, " Received : ", num_descriptor);
		throw std::bad_alloc();
	}
	auto DestDescriptorHandle = TableCache.BaseHandle;
	for (size_t i = 0; i < num_descriptor; i++)
	{
		DestDescriptorHandle[i] = CD3DX12_CPU_DESCRIPTOR_HANDLE(src, i, m_HeapIncrementSize);
	}

	m_StaleDescriptorMask |= (1 << rootindex);
}
void GPUDescriptorHeap::CommitStagedDescriptor(
	ID3D12GraphicsCommandList*														   commandlist,
	std::function<void(ID3D12GraphicsCommandList*, UINT, D3D12_GPU_DESCRIPTOR_HANDLE)> setFunc) noexcept
{
	auto NumOfDescriptorCommit = ComputeStaleDescriptorCount();

	if (NumOfDescriptorCommit)
	{
		auto Device = RenderEngine::GetDeviceD3D();
		// Make this return approiate commandlist based on framecount
		// auto CommandList = RenderEngine::GetCommandList();
		// Variable Condition
		auto FrameCount = RenderEngine::GetFenceValue();
		bool SelectEvenBank{ false };
		if (FrameCount % 2)
		{
			SelectEvenBank = true;
		}
		if (m_DynamicRegionSize < NumOfDescriptorCommit)
		{
			HAKU_LOG_WARN("Heap Resize : ", m_CurrentDescriptorHeapSize);
			m_CurrentDescriptorHeapSize = 2 * m_CurrentDescriptorHeapSize;
			ResetCacheAndDescriptorHeap();
			CreateDescriptorHeap();
			//TODO : might need to change this...
			commandlist->SetDescriptorHeaps(1, &m_VisibleHeap);
		}
		DWORD index;

		while (_BitScanForward64(&index, m_StaleDescriptorMask))
		{
			auto  NumDescriptor = m_DescriptorCache[index].NumberOfDescriptor;
			auto* SrcHandle		= m_DescriptorCache[index].BaseHandle;

			auto WorkingCPUHandle = m_OddCPUBaseHandle;
			auto WorkingGPUHandle = m_OddGPUBaseHandle;

			D3D12_CPU_DESCRIPTOR_HANDLE* pDestDescriptorRangeStarts	 = &m_OddCPUBaseHandle;
			UINT						 pDestDescriptorRangeSizes[] = { NumDescriptor };
			if (SelectEvenBank)
			{
				// if even bank is selected make it proper
				WorkingCPUHandle		   = m_EvenCPUBaseHandle;
				WorkingGPUHandle		   = m_EvenGPUBaseHandle;
				pDestDescriptorRangeStarts = &m_EvenCPUBaseHandle;
			}
			Device->CopyDescriptors(
				1, pDestDescriptorRangeStarts, pDestDescriptorRangeSizes, NumDescriptor, SrcHandle, nullptr, m_Type);
			// TODO : Get the appriate commandlist
			setFunc(commandlist, static_cast<UINT>(index), WorkingGPUHandle);

			WorkingCPUHandle.ptr += NumDescriptor * m_HeapIncrementSize;
			WorkingGPUHandle.ptr += NumDescriptor * m_HeapIncrementSize;

			m_StaleDescriptorMask ^= (1 << index);
		}
	}
}
void GPUDescriptorHeap::CreateDescriptorHeap()
{
	HAKU_LOG_WARN("Creating GPU Visible Heap : ", m_CurrentDescriptorHeapSize);
	m_DynamicRegionSize = static_cast<uint64_t>(0.45f * m_CurrentDescriptorHeapSize);
	m_StaticRegionSize	= (m_CurrentDescriptorHeapSize - (2 * m_DynamicRegionSize));

	HAKU_LOG_INFO("Static Region Size : ", m_StaticRegionSize);
	HAKU_LOG_INFO("Dynamic Region Size :", m_DynamicRegionSize);

	m_DescriptorHandleCache = new D3D12_CPU_DESCRIPTOR_HANDLE[m_CurrentDescriptorHeapSize];

	const auto Device = RenderEngine::GetDeviceD3D();

	D3D12_DESCRIPTOR_HEAP_DESC HeapDesc{};
	HeapDesc.Type			= m_Type;
	HeapDesc.NumDescriptors = m_CurrentDescriptorHeapSize;
	HeapDesc.Flags			= D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;

	HAKU_SOK_ASSERT(Device->CreateDescriptorHeap(
		&HeapDesc, __uuidof(ID3D12DescriptorHeap), reinterpret_cast<void**>(&m_VisibleHeap)))

	// Check if this is proper...
	// setting new handles for offset and the base handle
	m_CPUBaseHandle = m_VisibleHeap->GetCPUDescriptorHandleForHeapStart();
	m_GPUBaseHandle = m_VisibleHeap->GetGPUDescriptorHandleForHeapStart();

	m_EvenCPUBaseHandle.ptr = (m_CPUBaseHandle.ptr + (m_HeapIncrementSize * m_StaticRegionSize));
	m_EvenGPUBaseHandle.ptr = (m_GPUBaseHandle.ptr + (m_HeapIncrementSize * m_StaticRegionSize));

	m_OddCPUBaseHandle.ptr = (m_CPUBaseHandle.ptr + (m_HeapIncrementSize * (m_StaticRegionSize + m_DynamicRegionSize)));
	m_OddGPUBaseHandle.ptr = (m_GPUBaseHandle.ptr + (m_HeapIncrementSize * (m_StaticRegionSize + m_DynamicRegionSize)));
}
void GPUDescriptorHeap::ResetCacheAndDescriptorHeap() noexcept
{
	HAKU_LOG_WARN("Releasing The Cache and Descriptor Heap");
	if (!m_DescriptorHandleCache)
	{
		delete[] m_DescriptorHandleCache;
	}
	if (!m_VisibleHeap)
	{
		m_VisibleHeap->Release();
	}
}
uint32_t GPUDescriptorHeap::ComputeStaleDescriptorCount() noexcept
{
	uint32_t ret_val{ 0 };
	DWORD	 i;
	auto	 StaleMask = m_StaleDescriptorMask;
	while (_BitScanForward64(&i, StaleMask))
	{
		ret_val += m_DescriptorCache[i].NumberOfDescriptor;
		StaleMask ^= (1 << i);
	}
	return ret_val;
}
} // namespace Renderer
} // namespace Haku
