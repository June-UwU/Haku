#include "D3D12DescriptorHeaps.hpp"

namespace Haku
{
namespace Renderer
{
D3D12DescriptorHeap::D3D12DescriptorHeap(Haku::Renderer::D3D12RenderDevice& Device)
{
	D3D12_DESCRIPTOR_HEAP_DESC RTVdesc{};
	RTVdesc.NumDescriptors = FrameCount;
	RTVdesc.Type		   = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	RTVdesc.Flags		   = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	HAKU_SOK_ASSERT(Device.get()->CreateDescriptorHeap(&RTVdesc, IID_PPV_ARGS(&m_RtvHeap))) // check result
	D3D12_DESCRIPTOR_HEAP_DESC srvdesc{};
	srvdesc.Type		   = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvdesc.Flags		   = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvdesc.NumDescriptors = 6600;
	HAKU_SOK_ASSERT(Device.get()->CreateDescriptorHeap(&srvdesc, IID_PPV_ARGS(&m_SRVHeap)))
}
void D3D12DescriptorHeap::ShutDown() noexcept
{
	m_RtvHeap.Reset();
	m_SRVHeap.Reset();
}
D3D12_CPU_DESCRIPTOR_HANDLE D3D12DescriptorHeap::GetRTVCPUHandle() noexcept
{
	return m_RtvHeap->GetCPUDescriptorHandleForHeapStart();
}
D3D12_GPU_DESCRIPTOR_HANDLE D3D12DescriptorHeap::GetRTVGPUHandle() noexcept
{
	return m_RtvHeap->GetGPUDescriptorHandleForHeapStart();
}
D3D12_CPU_DESCRIPTOR_HANDLE D3D12DescriptorHeap::GetSRVCPUHandle() noexcept
{
	return m_SRVHeap->GetCPUDescriptorHandleForHeapStart();
}
D3D12_GPU_DESCRIPTOR_HANDLE D3D12DescriptorHeap::GetSRVGPUHandle() noexcept
{
	return m_SRVHeap->GetGPUDescriptorHandleForHeapStart();
}
void D3D12DescriptorHeap::SetDescriptorHeaps(D3D12CommandQueue& Command) noexcept
{
	ID3D12DescriptorHeap* ppHeaps[] = { m_SRVHeap.Get() };
	Command.GetCommandList()->SetDescriptorHeaps(_countof(ppHeaps), ppHeaps);
}
ID3D12DescriptorHeap* D3D12DescriptorHeap::GetSRVDescriptorHeap() noexcept
{
	return m_SRVHeap.Get();
}
} // namespace Renderer
} // namespace Haku
