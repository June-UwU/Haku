#include "D3D12SwapChain.hpp"

namespace Haku
{
namespace Renderer
{
D3D12SwapChain::D3D12SwapChain()
{
}
void D3D12SwapChain::Render()
{
	HAKU_SOK_ASSERT(m_SwapChain->Present(1, 0));
}

void D3D12SwapChain::Resize(
	uint32_t			 height,
	uint32_t			 width,
	D3D12RenderDevice&	 Device,
	D3D12CommandQueue&	 Command,
	D3D12DescriptorHeap& Heap)
{
	uint32_t ResizeHeight = std::max(8u, height);
	uint32_t ResizeWidth  = std::max(8u, width);
	Command.Synchronize();
	for (size_t i = 0; i < FrameCount; i++)
	{
		m_RenderTargets[i].Reset();
	}

	// D3D12_CLEAR_VALUE optimizedClearValue = {};
	// optimizedClearValue.Format = DXGI_FORMAT_D32_FLOAT;
	// optimizedClearValue.DepthStencil = { 1.0f, 0 };
	//
	// auto heapprop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	// auto texdesc = CD3DX12_RESOURCE_DESC::Tex2D(
	//	DXGI_FORMAT_D32_FLOAT, width, height, 1, 0, 1, 0, D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL);
	//
	// HAKU_SOK_ASSERT(Device.get()->CreateCommittedResource(
	//	&heapprop,
	//	D3D12_HEAP_FLAG_NONE,
	//	&texdesc,
	//	D3D12_RESOURCE_STATE_DEPTH_WRITE,
	//	&optimizedClearValue,
	//	IID_PPV_ARGS(&m_DSVResource)));

	DXGI_SWAP_CHAIN_DESC desc{};
	HAKU_SOK_ASSERT(m_SwapChain->GetDesc(&desc))
	HAKU_SOK_ASSERT(
		m_SwapChain->ResizeBuffers(FrameCount, ResizeWidth, ResizeHeight, desc.BufferDesc.Format, desc.Flags))
	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

	auto rtvDescriptorSize = Device.get()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(Heap.GetRTVCPUHandle());

	for (int i = 0; i < FrameCount; ++i)
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> backBuffer;
		HAKU_SOK_ASSERT(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)))

		Device.get()->CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);

		m_RenderTargets[i] = backBuffer;

		rtvHandle.Offset(rtvDescriptorSize);
	}
}

void D3D12SwapChain::SetAndClearRenderTarget(D3D12CommandQueue& Command, D3D12DescriptorHeap& Heap)
{
	// Indicate that the back buffer will be used as a render target.
	auto resbar = CD3DX12_RESOURCE_BARRIER::Transition(
		m_RenderTargets[m_FrameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	Command.GetCommandList()->ResourceBarrier(1, &resbar);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(Heap.GetRTVCPUHandle(), m_FrameIndex, m_RtvDescriptorSize);
	auto						  dsvhandle = Heap.GetDSVCPUHandle();

	Command.GetCommandList()->OMSetRenderTargets(1, &rtvHandle, FALSE, &dsvhandle);
	// clearing back buffer
	const float clearColor[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	Command.GetCommandList()->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	Command.GetCommandList()->ClearDepthStencilView(
		Heap.GetDSVCPUHandle(), D3D12_CLEAR_FLAG_DEPTH, 1.0f, 0, 0, nullptr);
}

void D3D12SwapChain::Init(
	Windows*			 Window,
	D3D12RenderDevice&	 Device,
	D3D12CommandQueue&	 Command,
	D3D12DescriptorHeap& Heap)
{
	Microsoft::WRL::ComPtr<IDXGIFactory6> DxgiFactory;
	HAKU_SOK_ASSERT(CreateDXGIFactory2(0, IID_PPV_ARGS(&DxgiFactory)))
	// Create Swapchain
	DXGI_SWAP_CHAIN_DESC1 Swap_desc{};
	Swap_desc.Height			 = Window->GetHeight();
	Swap_desc.Width				 = Window->GetWidth();
	Swap_desc.Format			 = DXGI_FORMAT_R8G8B8A8_UNORM;
	Swap_desc.BufferUsage		 = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	Swap_desc.BufferCount		 = FrameCount;
	Swap_desc.Scaling			 = DXGI_SCALING_NONE; // contrevesial movement
	Swap_desc.SwapEffect		 = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	Swap_desc.AlphaMode			 = DXGI_ALPHA_MODE_UNSPECIFIED; // mess around with the alpha
	Swap_desc.SampleDesc.Count	 = 1;
	Swap_desc.SampleDesc.Quality = 0;
	Microsoft::WRL::ComPtr<IDXGISwapChain1> swap_chain1;
	HAKU_SOK_ASSERT(DxgiFactory->CreateSwapChainForHwnd(
		Command.GetCommandQueue(), Window->GetHandle(), &Swap_desc, nullptr, nullptr, &swap_chain1))

	auto ret	 = swap_chain1.As(&m_SwapChain);
	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

	m_RtvDescriptorSize = Device.get()->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);
	///
	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE cpu_handle(Heap.GetRTVCPUHandle());
		// Creating a render target view(RTV)
		for (size_t i = 0; i < FrameCount; i++)
		{
			HAKU_SOK_ASSERT(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_RenderTargets[i])))
			Device.get()->CreateRenderTargetView(m_RenderTargets[i].Get(), nullptr, cpu_handle);
			cpu_handle.Offset(1, m_RtvDescriptorSize);
		}
	}

	D3D12_DEPTH_STENCIL_VIEW_DESC depthStencilDesc = {};
	depthStencilDesc.Format						   = DXGI_FORMAT_D32_FLOAT;
	depthStencilDesc.ViewDimension				   = D3D12_DSV_DIMENSION_TEXTURE2D;
	depthStencilDesc.Flags						   = D3D12_DSV_FLAG_NONE;
	depthStencilDesc.Texture2DArray.ArraySize	   = 2;

	D3D12_CLEAR_VALUE depthOptimizedClearValue	  = {};
	depthOptimizedClearValue.Format				  = DXGI_FORMAT_D32_FLOAT;
	depthOptimizedClearValue.DepthStencil.Depth	  = 1.0f;
	depthOptimizedClearValue.DepthStencil.Stencil = 0;

	auto				heapprop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	D3D12_RESOURCE_DESC texdesc{};
	texdesc.Dimension		   = D3D12_RESOURCE_DIMENSION_TEXTURE2D;
	texdesc.Alignment		   = 0;
	texdesc.Width			   = 4 * Window->GetWidth();
	texdesc.Height			   = 4 * Window->GetHeight();
	texdesc.DepthOrArraySize   = 2;
	texdesc.MipLevels		   = 0;
	texdesc.Format			   = DXGI_FORMAT_D32_FLOAT;
	texdesc.SampleDesc.Count   = 1;
	texdesc.SampleDesc.Quality = 0;
	texdesc.Flags			   = D3D12_RESOURCE_FLAG_ALLOW_DEPTH_STENCIL;

	HAKU_SOK_ASSERT(Device.get()->CreateCommittedResource(
		&heapprop,
		D3D12_HEAP_FLAG_NONE,
		&texdesc,
		D3D12_RESOURCE_STATE_DEPTH_WRITE,
		&depthOptimizedClearValue,
		IID_PPV_ARGS(&m_DSVResource)));

	Device.get()->CreateDepthStencilView(m_DSVResource.Get(), &depthStencilDesc, Heap.GetDSVCPUHandle());
}

void D3D12SwapChain::SetBackBufferIndex() noexcept
{
	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();
}

void D3D12SwapChain::ShutDown() noexcept
{
	m_SwapChain.Reset();
	for (size_t i = 0; i < FrameCount; i++)
	{
		m_RenderTargets[i].Reset();
	}
	m_DSVResource.Reset();
}

void D3D12SwapChain::TransitionPresent(D3D12CommandQueue& Command)
{
	auto resbarpres = CD3DX12_RESOURCE_BARRIER::Transition(
		m_RenderTargets[m_FrameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	Command.GetCommandList()->ResourceBarrier(1, &resbarpres);
}

} // namespace Renderer
} // namespace Haku