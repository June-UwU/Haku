#include "directx/d3dx12.h"
#include "D3D12Renderer.hpp"
#include "D3D12SwapChain.hpp"

// NOT A FINAL ABSTRACTION

namespace Haku
{
namespace Renderer
{
// D3D12SwapChain::D3D12SwapChain()
//{
//}
// void D3D12SwapChain::Render()
//{
//	HAKU_SOK_ASSERT(m_SwapChain->Present(1, 0));
//}
//
// void D3D12SwapChain::Resize(
//	uint32_t			 height,
//	uint32_t			 width,
//	D3D12RenderDevice&	 Device,
//	D3D12CommandQueue&	 Command,
//	D3D12DescriptorHeap& Heap)
//{
//	uint32_t ResizeHeight = math::max(8u, height);
//	uint32_t ResizeWidth  = math::max(8u, width);
//	Command.Synchronize();
//	for (size_t i = 0; i < FrameCount; i++)
//	{
//		m_RenderTargets[i].Reset();
//	}
//
//	DXGI_SWAP_CHAIN_DESC desc{};
//	HAKU_SOK_ASSERT(m_SwapChain->GetDesc(&desc))
//	HAKU_SOK_ASSERT(
//		m_SwapChain->ResizeBuffers(FrameCount, ResizeWidth, ResizeHeight, desc.BufferDesc.Format, desc.Flags))
//	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();
//
//	auto rtvDescriptorSize = Device.get()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
//
//	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(Heap.GetRTVCPUHandle());
//
//	for (int i = 0; i < FrameCount; ++i)
//	{
//		Microsoft::WRL::ComPtr<ID3D12Resource> backBuffer;
//		HAKU_SOK_ASSERT(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)))
//
//		Device.get()->CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);
//
//		HAKU_DXNAME(backBuffer, L"Render Target Resource")
//
//		m_RenderTargets[i] = backBuffer;
//
//		rtvHandle.Offset(rtvDescriptorSize);
//	}
//}
//
// void D3D12SwapChain::SetAndClearRenderTarget(D3D12CommandQueue& Command, D3D12DescriptorHeap& Heap)
//{
//	// Indicate that the back buffer will be used as a render target.
//	auto resbar = CD3DX12_RESOURCE_BARRIER::Transition(
//		m_RenderTargets[m_FrameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
//	Command.Get
//
//
//
// ()->ResourceBarrier(1, &resbar);
//
//	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(Heap.GetRTVCPUHandle(), m_FrameIndex, m_RtvDescriptorSize);
//	auto						  dsvhandle = Heap.GetDSVCPUHandle();
//
//	Command.Get
//
//
//
// ()->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvhandle);
//	// clearing back buffer
//	const float clearColor[] = { 0.2f, 0.2f, 0.2f, 1.0f };
//	Command.GetCommandList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
//	Command.GetCommandList()->ClearDepthStencilView(
//		Heap.GetDSVCPUHandle(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
//}
//
// void D3D12SwapChain::Init(
//	Windows*			 Window,
//	D3D12RenderDevice&	 Device,
//	D3D12CommandQueue&	 Command,
//	D3D12DescriptorHeap& Heap)
//{
//	Microsoft::WRL::ComPtr<IDXGIFactory6> DxgiFactory;
//	HAKU_SOK_ASSERT(CreateDXGIFactory2(0, IID_PPV_ARGS(&DxgiFactory)))
//	// Create Swapchain
//	DXGI_SWAP_CHAIN_DESC1 Swap_desc{};
//	Swap_desc.Height			 = Window->GetHeight();
//	Swap_desc.Width				 = Window->GetWidth();
//	Swap_desc.Format			 = DXGI_FORMAT_R8G8B8A8_UNORM;
//	Swap_desc.BufferUsage		 = DXGI_USAGE_RENDER_TARGET_OUTPUT;
//	Swap_desc.BufferCount		 = FrameCount;
//	Swap_desc.Scaling			 = DXGI_SCALING_NONE; // contrevesial movement
//	Swap_desc.SwapEffect		 = DXGI_SWAP_EFFECT_FLIP_DISCARD;
//	Swap_desc.AlphaMode			 = DXGI_ALPHA_MODE_UNSPECIFIED; // mess around with the alpha
//	Swap_desc.SampleDesc.Count	 = 1;
//	Swap_desc.SampleDesc.Quality = 0;
//	Microsoft::WRL::ComPtr<IDXGISwapChain1> swap_chain1;
//	HAKU_SOK_ASSERT(DxgiFactory->CreateSwapChainForHwnd(
//		Command.GetCommandQueue(), Window->GetHandle(), &Swap_desc, nullptr, nullptr, &swap_chain1))
//
//	auto ret	 = swap_chain1.As(&m_SwapChain);
//	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();
//
//	m_RtvDescriptorSize = Device.get()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
//	///
//	{
//		CD3DX12_CPU_DESCRIPTOR_HANDLE cpu_handle(Heap.GetRTVCPUHandle());
//		// Creating a render target view(RTV)
//		for (size_t i = 0; i < FrameCount; i++)
//		{
//			HAKU_SOK_ASSERT(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_RenderTargets[i])))
//			Device.get()->CreateRenderTargetView(m_RenderTargets[i].Get(), nullptr, cpu_handle);
//			cpu_handle.Offset(1, m_RtvDescriptorSize);
//		}
//	}
//
//	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
//	depthStencilDesc.Format						   = DXGI_FORMAT_D32_FLOAT;
//	depthStencilDesc.ViewDimension				   = D3D12_DSV_DIMENSION_TEXTURE2D;
//	depthStencilDesc.Flags						   = D3D12_DSV_FLAG_NONE;
//	depthStencilDesc.Texture2DArray.ArraySize	   = 2;
//
//	D3D12_CLEAR_VALUE depthOptimizedClearValue	  = {};
//	depthOptimizedClearValue.Format				  = DXGI_FORMAT_D32_FLOAT;
//	depthOptimizedClearValue.DepthStencil.Depth	  = 1.0f;
//	depthOptimizedClearValue.DepthStencil.Stencil = 0;
//
//	auto				heapprop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
//	D3D12_RESOURCE_DESC texdesc{};
//	texdesc.Dimension		   = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
//	texdesc.Alignment		   = 0;
//	texdesc.Width			   = 4 * Window->GetWidth();
//	texdesc.Height			   = 4 * Window->GetHeight();
//	texdesc.DepthOrArraySize   = 2;
//	texdesc.MipLevels		   = 0;
//	texdesc.Format			   = DXGI_FORMAT_D32_FLOAT;
//	texdesc.SampleDesc.Count   = 1;
//	texdesc.SampleDesc.Quality = 0;
//	texdesc.Flags			   = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;
//
//	HAKU_SOK_ASSERT(Device.get()->CreateCommittedResource(
//		&heapprop,
//		D3D12_HEAP_FLAG_NONE,
//		&texdesc,
//		D3D12_RESOURCE_STATE_DEPTH_WRITE,
//		&depthOptimizedClearValue,
//		IID_PPV_ARGS(&m_DSVResource)));
//
//	Device.get()->CreateDepthStencilView(m_DSVResource.Get(), &depthStencilDesc, Heap.GetDSVCPUHandle());
//	HAKU_DXNAME(m_DSVResource, L"Depth Stencil Resource");
//}
//
// void D3D12SwapChain::SetBackBufferIndex() noexcept
//{
//	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();
//}
//
// void D3D12SwapChain::ShutDown() noexcept
//{
//	m_SwapChain.Reset();
//	for (size_t i = 0; i < FrameCount; i++)
//	{
//		m_RenderTargets[i].Reset();
//	}
//	m_DSVResource.Reset();
//}
//
// void D3D12SwapChain::TransitionPresent(D3D12CommandQueue& Command)
//{
//	auto resbarpres = CD3DX12_RESOURCE_BARRIER::Transition(
//		m_RenderTargets[m_FrameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
//	Command.GetCommandList()->ResourceBarrier(1, &resbarpres);
//}

SwapChain::SwapChain()
	: m_DSVHeap(D3D12_DESCRIPTOR_HEAP_TYPE_DSV, 1)
	, m_RTVHeap(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, FrameCount)
{
}

SwapChain::~SwapChain()
{
}

void SwapChain::Present()
{
	m_SwapChain->Present(1, 0);
}

void SwapChain::Init(Windows* window)
{
	HAKU_LOG_INFO("Initializing Render SwapChain");
	auto								  Device = RenderEngine::GetDeviceD3D();
	auto								  Cmd	 = RenderEngine::GetCommandQueue();
	Microsoft::WRL::ComPtr<IDXGIFactory6> DxgiFactory;
	HAKU_SOK_ASSERT(CreateDXGIFactory2(0, IID_PPV_ARGS(&DxgiFactory)))
	// Create Swapchain
	DXGI_SWAP_CHAIN_DESC1 Swap_desc{};
	Swap_desc.SampleDesc.Count	 = 1;
	Swap_desc.SampleDesc.Quality = 0;
	Swap_desc.BufferCount		 = FrameCount;
	Swap_desc.Scaling			 = DXGI_SCALING_NONE; // contrevesial movement
	Swap_desc.Width				 = window->GetWidth();
	Swap_desc.Height			 = window->GetHeight();
	Swap_desc.Format			 = DXGI_FORMAT_R8G8B8A8_UNORM;
	Swap_desc.AlphaMode			 = DXGI_ALPHA_MODE_UNSPECIFIED; // mess around with the alpha
	Swap_desc.SwapEffect		 = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	Swap_desc.BufferUsage		 = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	Microsoft::WRL::ComPtr<IDXGISwapChain1> swap_chain1;
	HAKU_SOK_ASSERT(DxgiFactory->CreateSwapChainForHwnd(
		Cmd->Get(D3D12_COMMAND_LIST_TYPE_DIRECT), window->GetHandle(), &Swap_desc, nullptr, nullptr, &swap_chain1))

	HAKU_SOK_ASSERT(swap_chain1->QueryInterface(__uuidof(IDXGISwapChain3), reinterpret_cast<void**>(&m_SwapChain)))
	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

	m_RTVDescriptorHeapIncrementSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	///
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE cpu_handle(m_RTVHeap.GetCPUBaseHandle());
		// Creating a render target view(RTV)
		for (size_t i = 0; i < FrameCount; i++)
		{
			HAKU_SOK_ASSERT(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_RenderTargets[i])))
			Device->CreateRenderTargetView(m_RenderTargets[i], nullptr, cpu_handle);
			cpu_handle.Offset(1, m_RTVDescriptorHeapIncrementSize);

			wchar_t array[100]{};
			_snwprintf(array, 100, L"Back Buffer: %zd", i);
			HAKU_DXNAME(m_RenderTargets[i], array)
		}
	}

	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
	depthStencilDesc.Texture2DArray.ArraySize	   = 2;
	depthStencilDesc.Flags						   = D3D12_DSV_FLAG_NONE;
	depthStencilDesc.Format						   = DXGI_FORMAT_D32_FLOAT;
	depthStencilDesc.ViewDimension				   = D3D12_DSV_DIMENSION_TEXTURE2D;

	D3D12_CLEAR_VALUE depthOptimizedClearValue	  = {};
	depthOptimizedClearValue.DepthStencil.Stencil = 0;
	depthOptimizedClearValue.DepthStencil.Depth	  = 1.0f;
	depthOptimizedClearValue.Format				  = DXGI_FORMAT_D32_FLOAT;

	auto				heapprop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_RESOURCE_DESC texdesc{};
	texdesc.Alignment		   = 0;
	texdesc.SampleDesc.Quality = 0;
	texdesc.DepthOrArraySize   = 2;
	texdesc.MipLevels		   = 0;
	texdesc.SampleDesc.Count   = 1;
	texdesc.Format			   = DXGI_FORMAT_D32_FLOAT;
	texdesc.Width			   = 4 * window->GetWidth();
	texdesc.Height			   = 4 * window->GetHeight();
	texdesc.Dimension		   = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texdesc.Flags			   = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	HAKU_SOK_ASSERT(Device->CreateCommittedResource(
		&heapprop,
		D3D12_HEAP_FLAG_NONE,
		&texdesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthOptimizedClearValue,
		IID_PPV_ARGS(&m_DSVResource)));
	SetBackBufferIndex();
	Device->CreateDepthStencilView(m_DSVResource, &depthStencilDesc, m_DSVHeap.GetCPUBaseHandle());
	HAKU_DXNAME(m_DSVResource, L"Depth Stencil Resource");
}

uint64_t SwapChain::GetFrameIndex() noexcept
{
	std::lock_guard<std::mutex> lock(m_Mutex);
	return m_FrameIndex;
}

void SwapChain::SetBackBufferIndex() noexcept
{
	std::lock_guard<std::mutex> lock(m_Mutex);
	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();
}

void SwapChain::Resize(uint64_t height, uint64_t width)
{
	auto	 CommandQueue = RenderEngine::GetCommandQueue();
	auto	 Device		  = RenderEngine::GetDeviceD3D();
	uint32_t ResizeHeight = math::max(8u, height);
	uint32_t ResizeWidth  = math::max(8u, width);

	// Command.Synchronize();
	// making fence a independant class
	CommandQueue->Synchronize();
	for (size_t i = 0; i < FrameCount; i++)
	{
		m_RenderTargets[i]->Release();
	}

	DXGI_SWAP_CHAIN_DESC desc{};
	HAKU_SOK_ASSERT(m_SwapChain->GetDesc(&desc))
	HAKU_SOK_ASSERT(
		m_SwapChain->ResizeBuffers(FrameCount, ResizeWidth, ResizeHeight, desc.BufferDesc.Format, desc.Flags))
	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

	auto rtvDescriptorSize = Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RTVHeap.GetCPUBaseHandle());

	for (int i = 0; i < FrameCount; ++i)
	{
		ID3D12Resource* backBuffer;
		HAKU_SOK_ASSERT(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)))

		Device->CreateRenderTargetView(backBuffer, nullptr, rtvHandle);

		m_RenderTargets[i] = backBuffer;

		rtvHandle.Offset(rtvDescriptorSize);
	}
}

ID3D12Resource* SwapChain::GetRenderTargetResource(size_t pos) noexcept
{
	assert(pos < FrameCount);
	return m_RenderTargets[pos];
}

D3D12_CPU_DESCRIPTOR_HANDLE& SwapChain::GetCPUHeapHandle(D3D12_DESCRIPTOR_HEAP_TYPE type)
{
	switch (type)
	{
	case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
	{
		return m_DSVHeap.GetCPUBaseHandle();
	}
	case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
	{
		return m_RTVHeap.GetCPUBaseHandle();
	}
	default:
	{
		HAKU_LOG_CRIT("Unknown Swapchain cpu heap type :", type);
		throw std::logic_error("Unknown Swapchain cpu heap type");
	}
	}
}

D3D12_GPU_DESCRIPTOR_HANDLE& SwapChain::GetGPUHeapHandle(D3D12_DESCRIPTOR_HEAP_TYPE type)
{
	switch (type)
	{
	case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
	{
		return m_DSVHeap.GetGPUBaseHandle();
	}
	case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
	{
		return m_RTVHeap.GetGPUBaseHandle();
	}
	default:
	{
		HAKU_LOG_CRIT("Unknown Swapchain gpu heap type :", type);
		throw std::logic_error("Unknown Swapchain gpu heap type");
	}
	}
}

D3D12_CPU_DESCRIPTOR_HANDLE SwapChain::GetCPUHeapHandleInOffset(D3D12_DESCRIPTOR_HEAP_TYPE type, size_t pos)
{
	switch (type)
	{
	case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
	{
		return m_DSVHeap.GetCPUHandleOnPosition(pos);
	}
	case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
	{
		return m_RTVHeap.GetCPUHandleOnPosition(pos);
	}
	default:
	{
		HAKU_LOG_CRIT("Unknown Swapchain gpu heap type :", type);
		throw std::logic_error("Unknown Swapchain gpu heap type");
	}
	}
}

D3D12_GPU_DESCRIPTOR_HANDLE SwapChain::GetGPUHeapHandleInOffset(D3D12_DESCRIPTOR_HEAP_TYPE type, size_t pos)
{
	switch (type)
	{
	case D3D12_DESCRIPTOR_HEAP_TYPE_DSV:
	{
		return m_DSVHeap.GetGPUHandleOnPosition(pos);
	}
	case D3D12_DESCRIPTOR_HEAP_TYPE_RTV:
	{
		return m_RTVHeap.GetGPUHandleOnPosition(pos);
	}
	default:
	{
		HAKU_LOG_CRIT("Unknown Swapchain gpu heap type :", type);
		throw std::logic_error("Unknown Swapchain gpu heap type");
	}
	}
}

void SwapChain::ShutDown() noexcept
{
	m_SwapChain->Release();
	for (size_t i = 0; i < FrameCount; i++)
	{
		m_RenderTargets[i]->Release();
	}
	m_DSVResource->Release();
}

} // namespace Renderer
} // namespace Haku
