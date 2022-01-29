#pragma once
#include "../macros.hpp"
#include "directx/d3d12.h"
#include "directx/d3dx12.h"
#include "D3D12RenderDevice.hpp"
#include "D3D12CommandQueue.hpp"
#include "../Core/Exceptions.hpp"

/// moving only the Rtv heap at first...we get triangle we add srv_cbv and eventually dsv

namespace Haku
{
namespace Renderer
{
class D3D12DescriptorHeap
{
public:
	D3D12DescriptorHeap(D3D12RenderDevice& Device);
	void						ShutDown() noexcept;
	D3D12_CPU_DESCRIPTOR_HANDLE GetRTVCPUHandle() noexcept;
	D3D12_GPU_DESCRIPTOR_HANDLE GetRTVGPUHandle() noexcept;
	D3D12_CPU_DESCRIPTOR_HANDLE GetSRVCPUHandle() noexcept;
	D3D12_GPU_DESCRIPTOR_HANDLE GetSRVGPUHandle() noexcept;
	D3D12_CPU_DESCRIPTOR_HANDLE GetDSVCPUHandle() noexcept;
	D3D12_GPU_DESCRIPTOR_HANDLE GetDSVGPUHandle() noexcept;
	ID3D12DescriptorHeap*		GetSRVDescriptorHeap() noexcept;
	ID3D12DescriptorHeap*		GetRTVDescriptorHeap() noexcept;
	ID3D12DescriptorHeap*		GetDSVDescriptorHeap() noexcept;
	void						SetDescriptorHeaps(D3D12CommandQueue& Command) noexcept;

private:
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_RTVHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_SRVHeap;
	Microsoft::WRL::ComPtr<ID3D12DescriptorHeap> m_DSVHeap;
};
} // namespace Renderer
} // namespace Haku
