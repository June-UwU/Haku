#include "../../macros.hpp"
#include "D3D12RenderDevice.hpp"
#include "D3D12CommandQueue.hpp"
#include "../../Core/HakuLog.hpp"
#include "../../Core/Exceptions.hpp"
namespace Haku
{
namespace Renderer
{
D3D12RenderDevice::D3D12RenderDevice()
{
	unsigned int FactoryFlags{};
#ifdef _DEBUG
	{
		Microsoft::WRL::ComPtr<ID3D12Debug> DebugDevice;
		HAKU_SOK_ASSERT(D3D12GetDebugInterface(IID_PPV_ARGS(&DebugDevice)))
		FactoryFlags |= DXGI_CREATE_FACTORY_DEBUG;
		DebugDevice->EnableDebugLayer();
	}
#endif
	Microsoft::WRL::ComPtr<IDXGIAdapter> GraphicsAdapter; // Graphics adapter IUnknown
	Microsoft::WRL::ComPtr<IDXGIFactory6>
		DxgiFactory; // this might fail due to lack of support,might need to extend the interface
	HAKU_SOK_ASSERT(CreateDXGIFactory2(FactoryFlags, IID_PPV_ARGS(&DxgiFactory)))
	HAKU_SOK_ASSERT(DxgiFactory->EnumAdapterByGpuPreference(
		0, DXGI_GPU_PREFERENCE_HIGH_PERFORMANCE, IID_PPV_ARGS(&GraphicsAdapter)))

	DXGI_ADAPTER_DESC adpter_desc{};
	HAKU_SOK_ASSERT(GraphicsAdapter->GetDesc(&adpter_desc))

	HAKU_SOK_ASSERT(D3D12CreateDevice(
		GraphicsAdapter.Get(),
		D3D_FEATURE_LEVEL_12_0,
		IID_PPV_ARGS(&m_Device))) // the support and request needs checking
}
void D3D12RenderDevice::Render()
{
	HAKU_SOK_ASSERT(m_SwapChain->Present(1, 0));
}
void D3D12RenderDevice::FrameIndexReset()
{
	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();
}
void D3D12RenderDevice::RenderTarget(ID3D12GraphicsCommandList* list)
{
	// Indicate that the back buffer will be used as a render target.
	auto resbar = CD3DX12_RESOURCE_BARRIER::Transition(
		m_RenderTargets[m_FrameIndex].Get(), D3D12_RESOURCE_STATE_PRESENT, D3D12_RESOURCE_STATE_RENDER_TARGET);
	list->ResourceBarrier(1, &resbar);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
		m_RtvHeap->GetCPUDescriptorHandleForHeapStart(), m_FrameIndex, m_RtvDescriptorSize);
	list->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
	// clearing back buffer
	const float clearColor[] = { 0.2f, 0.2f, 0.2f, 1.0f };
	list->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
}
void D3D12RenderDevice::init(Haku::Windows* window, ID3D12CommandQueue* CommandQueue)
{
	Microsoft::WRL::ComPtr<IDXGIFactory6> DxgiFactory;
	HAKU_SOK_ASSERT(CreateDXGIFactory2(0, IID_PPV_ARGS(&DxgiFactory)))
	// Create Swapchain
	DXGI_SWAP_CHAIN_DESC1 Swap_desc{};
	Swap_desc.Height			 = window->GetHeight();
	Swap_desc.Width				 = window->GetWidth();
	Swap_desc.Format			 = DXGI_FORMAT_R8G8B8A8_UNORM;
	Swap_desc.Stereo			 = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	Swap_desc.BufferUsage		 = DXGI_USAGE_RENDER_TARGET_OUTPUT;
	Swap_desc.BufferCount		 = FrameCount;
	Swap_desc.Scaling			 = DXGI_SCALING_NONE; // contrevesial movement
	Swap_desc.SwapEffect		 = DXGI_SWAP_EFFECT_FLIP_DISCARD;
	Swap_desc.AlphaMode			 = DXGI_ALPHA_MODE_UNSPECIFIED; // mess around with the alpha
	Swap_desc.SampleDesc.Count	 = 1;
	Swap_desc.SampleDesc.Quality = 0;

	// basic swapchain1  interface
	Microsoft::WRL::ComPtr<IDXGISwapChain1> swap_chain1;
	HAKU_SOK_ASSERT(DxgiFactory->CreateSwapChainForHwnd(
		CommandQueue, window->GetHandle(), &Swap_desc, nullptr, nullptr, &swap_chain1))
	// Extending the swapchain interface
	HAKU_SOK_ASSERT(swap_chain1.As(&m_SwapChain))
	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

	D3D12_DESCRIPTOR_HEAP_DESC RTVdesc{};
	RTVdesc.Type		   = D3D12_DESCRIPTOR_HEAP_TYPE_RTV;
	RTVdesc.NumDescriptors = FrameCount;
	RTVdesc.Flags		   = D3D12_DESCRIPTOR_HEAP_FLAG_NONE;
	HAKU_SOK_ASSERT(m_Device->CreateDescriptorHeap(&RTVdesc, IID_PPV_ARGS(&m_RtvHeap)))
	m_RtvDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	{
		CD3DX12_CPU_DESCRIPTOR_HANDLE cpu_handle(m_RtvHeap->GetCPUDescriptorHandleForHeapStart());
		// Creating a render target view(RTV)
		for (size_t i = 0; i < FrameCount; i++)
		{
			HAKU_SOK_ASSERT(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&m_RenderTargets[i])))
			m_Device->CreateRenderTargetView(m_RenderTargets[i].Get(), nullptr, cpu_handle);
			cpu_handle.Offset(1, m_RtvDescriptorSize);
		}
	}
}
void D3D12RenderDevice::Resize(uint32_t height, uint32_t width, D3D12CommandQueue& Command)
{
	uint32_t ResizeHeight = std::max(1u, height);
	uint32_t ResizeWidth  = std::max(1u, width);
	Command.Synchronize();
	for (size_t i = 0; i < FrameCount; i++)
	{
		m_RenderTargets[i].Reset();
	}
	DXGI_SWAP_CHAIN_DESC desc{};
	HAKU_SOK_ASSERT(m_SwapChain->GetDesc(&desc))
	HAKU_SOK_ASSERT(m_SwapChain->ResizeBuffers(FrameCount, width, height, desc.BufferDesc.Format, desc.Flags))
	m_FrameIndex = m_SwapChain->GetCurrentBackBufferIndex();

	auto rtvDescriptorSize = m_Device->GetDescriptorHandleIncrementSize(D3D12_DESCRIPTOR_HEAP_TYPE_RTV);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(m_RtvHeap->GetCPUDescriptorHandleForHeapStart());

	for (int i = 0; i < FrameCount; ++i)
	{
		Microsoft::WRL::ComPtr<ID3D12Resource> backBuffer;
		HAKU_SOK_ASSERT(m_SwapChain->GetBuffer(i, IID_PPV_ARGS(&backBuffer)))

		m_Device->CreateRenderTargetView(backBuffer.Get(), nullptr, rtvHandle);

		m_RenderTargets[i] = backBuffer;

		rtvHandle.Offset(rtvDescriptorSize);
	}
}
void D3D12RenderDevice::BackBuffer(ID3D12GraphicsCommandList* list)
{
	auto resbarpres = CD3DX12_RESOURCE_BARRIER::Transition(
		m_RenderTargets[m_FrameIndex].Get(), D3D12_RESOURCE_STATE_RENDER_TARGET, D3D12_RESOURCE_STATE_PRESENT);
	list->ResourceBarrier(1, &resbarpres);
}
} // namespace Renderer
} // namespace Haku
