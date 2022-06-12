#include "D3D12Renderer.hpp"
#include "D3D12DescriptorHeaps.hpp"

namespace Haku
{
namespace Renderer
{
// D3D12DescriptorHeap::D3D12DescriptorHeap(Haku::Renderer::D3D12RenderDevice& Device)
//{
//	D3D12_DESCRIPTOR_HEAP_DESC RTVdesc{};
//	RTVdesc.NumDescriptors = FrameCount;
//	RTVdesc.Type		   = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
//	RTVdesc.Flags		   = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
//
//	HAKU_SOK_ASSERT(Device.get()->CreateDescriptorHeap(&RTVdesc, IID_PPV_ARGS(&m_RTVHeap))) // check result
//	D3D12_DESCRIPTOR_HEAP_DESC srvdesc{};
//	srvdesc.Type		   = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
//	srvdesc.Flags		   = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
//	srvdesc.NumDescriptors = 6600;
//	HAKU_SOK_ASSERT(Device.get()->CreateDescriptorHeap(&srvdesc, IID_PPV_ARGS(&m_SRVHeap)))
//
//	D3D12_DESCRIPTOR_HEAP_DESC DSVdesc{};
//	DSVdesc.NumDescriptors = 1;
//	DSVdesc.Type		   = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
//	DSVdesc.Flags		   = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
//
//	HAKU_SOK_ASSERT(Device.get()->CreateDescriptorHeap(&DSVdesc, IID_PPV_ARGS(&m_DSVHeap)))
//	HAKU_DXNAME(m_RTVHeap, L"RTV Heap")
//	HAKU_DXNAME(m_SRVHeap, L"SRV Heap")
//	HAKU_DXNAME(m_DSVHeap, L"DSV Heap")
//}
// void D3D12DescriptorHeap::ShutDown() noexcept
//{
//	m_RTVHeap.Reset();
//	m_SRVHeap.Reset();
//	m_DSVHeap.Reset();
//}
// D3D12_CPU_DESCRIPTOR_HANDLE D3D12DescriptorHeap::GetRTVCPUHandle() noexcept
//{
//	return m_RTVHeap->GetCPUDescriptorHandleForHeapStart();
//}
// D3D12_GPU_DESCRIPTOR_HANDLE D3D12DescriptorHeap::GetRTVGPUHandle() noexcept
//{
//	return m_RTVHeap->GetGPUDescriptorHandleForHeapStart();
//}
// D3D12_CPU_DESCRIPTOR_HANDLE D3D12DescriptorHeap::GetSRVCPUHandle() noexcept
//{
//	return m_SRVHeap->GetCPUDescriptorHandleForHeapStart();
//}
// D3D12_GPU_DESCRIPTOR_HANDLE D3D12DescriptorHeap::GetSRVGPUHandle() noexcept
//{
//	return m_SRVHeap->GetGPUDescriptorHandleForHeapStart();
//}
// D3D12_CPU_DESCRIPTOR_HANDLE D3D12DescriptorHeap::GetDSVCPUHandle() noexcept
//{
//	return m_DSVHeap->GetCPUDescriptorHandleForHeapStart();
//}
// D3D12_GPU_DESCRIPTOR_HANDLE D3D12DescriptorHeap::GetDSVGPUHandle() noexcept
//{
//	return m_DSVHeap->GetGPUDescriptorHandleForHeapStart();
//}
// void D3D12DescriptorHeap::SetDescriptorHeaps(D3D12CommandQueue& Command) noexcept
//{
//	ID3D12DescriptorHeap* ppHeaps[] = { m_SRVHeap.Get() };
//	Command.Get
//
//
// ()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
//}
//
// ID3D12DescriptorHeap* D3D12DescriptorHeap::GetSRVDescriptorHeap() noexcept
//{
//	return m_SRVHeap.Get();
//}
// ID3D12DescriptorHeap* D3D12DescriptorHeap::GetRTVDescriptorHeap() noexcept
//{
//	return m_RTVHeap.Get();
//}
// ID3D12DescriptorHeap* D3D12DescriptorHeap::GetDSVDescriptorHeap() noexcept
//{
//	return m_DSVHeap.Get();
//}

DescriptorHeap::DescriptorHeap(
	D3D12_DESCRIPTOR_HEAP_TYPE	type,
	uint64_t					number_of_descriptors,
	D3D12_DESCRIPTOR_HEAP_FLAGS flags)
{
	D3D12_DESCRIPTOR_HEAP_DESC desc{};
	desc.NodeMask		= 0;
	desc.Type			= type;
	desc.Flags			= flags;
	desc.NumDescriptors = number_of_descriptors;

	auto Device = RenderEngine::GetDeviceD3D();

	HAKU_LOG_INFO("Creating Desriptor Heap", type, "Descriptor Count : ", number_of_descriptors);
	HAKU_SOK_ASSERT(Device->CreateDescriptorHeap(&desc, IID_PPV_ARGS(&m_Heap)));
	m_CPUHandle			= m_Heap->GetCPUDescriptorHandleForHeapStart();
	m_GPUHandle			= m_Heap->GetGPUDescriptorHandleForHeapStart();
	m_IncrementSize = Device->GetDescriptorHandleIncrementSize(type);
}
DescriptorHeap::~DescriptorHeap() noexcept
{
	ShutDown();
}
const ID3D12DescriptorHeap* DescriptorHeap::Get() noexcept
{
	return m_Heap;
}
D3D12_CPU_DESCRIPTOR_HANDLE& DescriptorHeap::GetCPUBaseHandle() noexcept
{
	return m_CPUHandle;
}
D3D12_GPU_DESCRIPTOR_HANDLE& DescriptorHeap::GetGPUBaseHandle() noexcept
{
	return m_GPUHandle;
}
D3D12_CPU_DESCRIPTOR_HANDLE DescriptorHeap::GetCPUHandleOnPosition(size_t pos) noexcept
{
	return CD3DX12_CPU_DESCRIPTOR_HANDLE(m_CPUHandle, pos, m_IncrementSize);
}
D3D12_GPU_DESCRIPTOR_HANDLE DescriptorHeap::GetGPUHandleOnPosition(size_t pos) noexcept
{
	return CD3DX12_GPU_DESCRIPTOR_HANDLE(m_GPUHandle, pos, m_IncrementSize);
}
void DescriptorHeap::ShutDown() noexcept
{
	m_Heap->Release();
}
} // namespace Renderer
} // namespace Haku
