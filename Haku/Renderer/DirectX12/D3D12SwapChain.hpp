#pragma once

#include <mutex>
#include "macros.hpp"
#include <dxgi1_4.h>
#include "hakupch.hpp"
#include "directx/d3d12.h"
#include "Core/HakuLog.hpp"
#include "Core/Exceptions.hpp"
#include "D3D12DescriptorHeaps.hpp"

#include "wil/com.h"
#include "wil/wrl.h"
#include "D3D12CommandQueue.hpp"
#include "D3D12RenderDevice.hpp"
#include "Platform/Windows/MainWindow.hpp"

namespace Haku
{
namespace Renderer
{
class SwapChain
{
public:
	SwapChain();
	DISABLE_COPY(SwapChain)
	DISABLE_MOVE(SwapChain)
	~SwapChain();
	void						 Present();
	void						 ShutDown() noexcept;
	void						 Init(Windows* window);
	uint64_t					 GetFrameIndex() noexcept;
	void						 SetBackBufferIndex() noexcept; // method for the fence to set back buffer index
	void						 Resize(uint64_t height, uint64_t width);
	ID3D12Resource*				 GetRenderTargetResource(size_t pos) noexcept;
	D3D12_CPU_DESCRIPTOR_HANDLE& GetCPUHeapHandle(D3D12_DESCRIPTOR_HEAP_TYPE type);
	D3D12_GPU_DESCRIPTOR_HANDLE& GetGPUHeapHandle(D3D12_DESCRIPTOR_HEAP_TYPE type);
	D3D12_CPU_DESCRIPTOR_HANDLE	 GetCPUHeapHandleInOffset(D3D12_DESCRIPTOR_HEAP_TYPE type, size_t pos);
	D3D12_GPU_DESCRIPTOR_HANDLE	 GetGPUHeapHandleInOffset(D3D12_DESCRIPTOR_HEAP_TYPE type, size_t pos);

private:
	std::mutex								m_Mutex;
	DescriptorHeap							m_DSVHeap;
	DescriptorHeap							m_RTVHeap;
	IDXGISwapChain3*						m_SwapChain;
	uint64_t								m_FrameIndex;
	ID3D12Resource*							m_DSVResource;
	std::array<ID3D12Resource*, FrameCount> m_RenderTargets;
	uint64_t								m_RTVDescriptorHeapIncrementSize;
};

/// DEPRECIATING CLASS
// class D3D12SwapChain
//{
// public:
//	D3D12SwapChain();
//	void Render();
//	void ShutDown() noexcept;
//	void SetBackBufferIndex() noexcept;
//	void TransitionPresent(D3D12CommandQueue& Command);
//	void Resize(
//		uint32_t			 height,
//		uint32_t			 width,
//		D3D12RenderDevice&	 Device,
//		D3D12CommandQueue&	 Command,
//		D3D12DescriptorHeap& Heap);
//	void SetAndClearRenderTarget(D3D12CommandQueue& Command, D3D12DescriptorHeap& Heap);
//	void Init(Windows* Window, D3D12RenderDevice& Device, D3D12CommandQueue& Command, D3D12DescriptorHeap& Heap);
//
// private:
//	Microsoft::WRL::ComPtr<IDXGISwapChain3> m_SwapChain;
//	Microsoft::WRL::ComPtr<ID3D12Resource>	m_DSVResource;
//	uint32_t								m_FrameIndex		= 0;
//	uint32_t								m_RtvDescriptorSize = 0;
//	Microsoft::WRL::ComPtr<ID3D12Resource>	m_RenderTargets[FrameCount];
//};
} // namespace Renderer
} // namespace Haku
