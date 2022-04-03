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

	HAKU_SOK_ASSERT(Device.get()->CreateDescriptorHeap(&RTVdesc, IID_PPV_ARGS(&m_RTVHeap))) // check result
	D3D12_DESCRIPTOR_HEAP_DESC srvdesc{};
	srvdesc.Type		   = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV;
	srvdesc.Flags		   = D3D12_DESCRIPTOR_HEAP_FLAG_SHADER_VISIBLE;
	srvdesc.NumDescriptors = 6600;
	HAKU_SOK_ASSERT(Device.get()->CreateDescriptorHeap(&srvdesc, IID_PPV_ARGS(&m_SRVHeap)))

	D3D12_DESCRIPTOR_HEAP_DESC DSVdesc{};
	DSVdesc.NumDescriptors = 1;
	DSVdesc.Type		   = D3D12_DESCRIPTOR_HEAP_TYPE_DSV;
	DSVdesc.Flags		   = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;

	HAKU_SOK_ASSERT(Device.get()->CreateDescriptorHeap(&DSVdesc, IID_PPV_ARGS(&m_DSVHeap)))
	HAKU_DXNAME(m_RTVHeap,L"RTV Heap")
	HAKU_DXNAME(m_SRVHeap,L"SRV Heap")
	HAKU_DXNAME(m_DSVHeap,L"DSV Heap")
}
void D3D12DescriptorHeap::ShutDown() noexcept
{
	m_RTVHeap.Reset();
	m_SRVHeap.Reset();
	m_DSVHeap.Reset();
}
D3D12_CPU_DESCRIPTOR_HANDLE D3D12DescriptorHeap::GetRTVCPUHandle() noexcept
{
	return m_RTVHeap->GetCPUDescriptorHandleForHeapStart();
}
D3D12_GPU_DESCRIPTOR_HANDLE D3D12DescriptorHeap::GetRTVGPUHandle() noexcept
{
	return m_RTVHeap->GetGPUDescriptorHandleForHeapStart();
}
D3D12_CPU_DESCRIPTOR_HANDLE D3D12DescriptorHeap::GetSRVCPUHandle() noexcept
{
	return m_SRVHeap->GetCPUDescriptorHandleForHeapStart();
}
D3D12_GPU_DESCRIPTOR_HANDLE D3D12DescriptorHeap::GetSRVGPUHandle() noexcept
{
	return m_SRVHeap->GetGPUDescriptorHandleForHeapStart();
}
D3D12_CPU_DESCRIPTOR_HANDLE D3D12DescriptorHeap::GetDSVCPUHandle() noexcept
{
	return m_DSVHeap->GetCPUDescriptorHandleForHeapStart();
}
D3D12_GPU_DESCRIPTOR_HANDLE D3D12DescriptorHeap::GetDSVGPUHandle() noexcept
{
	return m_DSVHeap->GetGPUDescriptorHandleForHeapStart();
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
ID3D12DescriptorHeap* D3D12DescriptorHeap::GetRTVDescriptorHeap() noexcept
{
	return m_RTVHeap.Get();
}
ID3D12DescriptorHeap* D3D12DescriptorHeap::GetDSVDescriptorHeap() noexcept
{
	return m_DSVHeap.Get();
}
} // namespace Renderer
} // namespace Haku
