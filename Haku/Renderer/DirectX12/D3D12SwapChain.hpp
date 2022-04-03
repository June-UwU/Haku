#pragma once
#include "wil/com.h"
#include "wil/wrl.h"
#include <dxgi1_4.h>
#include "../../macros.hpp"
#include "../../hakupch.hpp"
#include "directx/d3d12.h"
#include "directx/d3dx12.h"
#include "../../Core/HakuLog.hpp"
#include "D3D12CommandQueue.hpp"
#include "D3D12RenderDevice.hpp"
#include "../../Core/Exceptions.hpp"
#include "D3D12DescriptorHeaps.hpp"
#include "../../Platform/Windows/MainWindow.hpp"

namespace Haku
{
namespace Renderer
{
class D3D12SwapChain
{
public:
	D3D12SwapChain();
	void Render();
	void ShutDown() noexcept;
	void SetBackBufferIndex() noexcept;
	void TransitionPresent(D3D12CommandQueue& Command);
	void Resize(
		uint32_t			 height,
		uint32_t			 width,
		D3D12RenderDevice&	 Device,
		D3D12CommandQueue&	 Command,
		D3D12DescriptorHeap& Heap);
	void SetAndClearRenderTarget(D3D12CommandQueue& Command, D3D12DescriptorHeap& Heap);
	void Init(Windows* Window, D3D12RenderDevice& Device, D3D12CommandQueue& Command, D3D12DescriptorHeap& Heap);

private:
	Microsoft::WRL::ComPtr<IDXGISwapChain3> m_SwapChain;
	Microsoft::WRL::ComPtr<ID3D12Resource>	m_DSVResource;
	uint32_t								m_FrameIndex		= 0;
	uint32_t								m_RtvDescriptorSize = 0;
	Microsoft::WRL::ComPtr<ID3D12Resource>	m_RenderTargets[FrameCount];
};
} // namespace Renderer
} // namespace Haku
