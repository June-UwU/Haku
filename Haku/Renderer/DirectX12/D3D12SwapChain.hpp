#pragma once

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
	void Present();
	void Init(Windows* window);
	void SetBackBufferIndex() noexcept; //method for the fence to set back buffer index
	void Resize(uint64_t height, uint64_t width);
	// These two are probably commandlist methods
	// void ClearRenderTargets() noexcept;
	/// would probably thanos this function
	// void TransitionPresent();

private:
	void ShutDown() noexcept;

private:
	DescriptorHeap							m_DSVHeap;
	DescriptorHeap							m_RTVHeap;
	Microsoft::WRL::ComPtr<IDXGISwapChain3> m_SwapChain;
	uint64_t								m_FrameIndex;
	ID3D12Resource*							m_DSVResource;
	ID3D12Resource*							m_RenderTargets[FrameCount];
	uint64_t								m_RTVDescriptorHeapIncrementSize;
};

/// DEPRECIATING CLASS
//class D3D12SwapChain
//{
//public:
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
//private:
//	Microsoft::WRL::ComPtr<IDXGISwapChain3> m_SwapChain;
//	Microsoft::WRL::ComPtr<ID3D12Resource>	m_DSVResource;
//	uint32_t								m_FrameIndex		= 0;
//	uint32_t								m_RtvDescriptorSize = 0;
//	Microsoft::WRL::ComPtr<ID3D12Resource>	m_RenderTargets[FrameCount];
//};
} // namespace Renderer
} // namespace Haku
