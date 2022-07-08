#include "directx/d3d12.h"
#include "d3dcompiler.h"
#include "Core/UILayer.hpp"
#include "D3D12Renderer.hpp"
#include "Core/Exceptions.hpp"
#include "Core/Application.hpp"
#include "..\..\imgui\backends\imgui_impl_dx12.h"

#define TEST 1
#pragma comment(lib, "DXGI.lib")
#pragma comment(lib, "D3D12.lib")
#pragma comment(lib, "dxguid.lib")
#pragma comment(lib, "D3DCompiler.lib")

namespace Haku
{
namespace Renderer
{
std::mutex						RenderEngine::S_mutex;
std::unique_ptr<Fence>			RenderEngine::S_Fence;
std::unique_ptr<RenderDevice>	RenderEngine::S_Device;
std::unique_ptr<SwapChain>		RenderEngine::S_SwapChain;
std::unique_ptr<D3D12_VIEWPORT> RenderEngine::S_Viewport;
std::atomic_uint64_t			RenderEngine::S_FenceValue;
std::unique_ptr<CommandQueue>	RenderEngine::S_CommandQueue;
std::unique_ptr<D3D12_RECT>		RenderEngine::S_ScissorRect;
CommandList*					RenderEngine::S_CurrentCommandList;
std::array<std::unique_ptr<DescriptorAllocator>, D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES>
	RenderEngine::S_CPUDescriptorAllocators;

void RenderEngine::EndScene()
{
	S_SwapChain->Present();
	S_CommandQueue->Synchronize();
	S_SwapChain->SetBackBufferIndex();
	S_CurrentCommandList = nullptr;
};
void RenderEngine::BeginScene()
{
	S_CurrentCommandList = RequestCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT);
	S_CurrentCommandList->Reset();
	S_CurrentCommandList->TransitionSwapChainRenderTarget();
	S_CurrentCommandList->IASetPrimitiveTopology(D3D_PRIMITIVE_TOPOLOGY_TRIANGLELIST);
	S_CurrentCommandList->RSSetScissorRects(1, S_ScissorRect.get());
	S_CurrentCommandList->RSSetViewports(1, S_Viewport.get());
	CD3DX12_CPU_DESCRIPTOR_HANDLE Handle(
		S_SwapChain->GetCPUHeapHandleInOffset(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, S_SwapChain->GetFrameIndex()));
	auto dsvhandle = S_SwapChain->GetCPUHeapHandle(D3D12_DESCRIPTOR_HEAP_TYPE_DSV);
	S_CurrentCommandList->OMSetRenderTargets(1, &Handle, FALSE, &dsvhandle);
	const float clearColor[] = { 0.1f, 0.1f, 0.1f, 1.0f };
	S_CurrentCommandList->ClearRenderTargetView(Handle, clearColor, 0, nullptr);
};
void RenderEngine::Render()
{
	BeginScene();
	RecordAndCommandList();
	S_CommandQueue->ExecuteLists(S_CurrentCommandList);
	EndScene();
}
void RenderEngine::ResizeEvent(uint64_t height, uint64_t width)
{
	S_SwapChain->Resize(height, width);
}

void RenderEngine::ShutDown()
{
	// Synchronzing to make all command list finish execution
	S_CommandQueue->Synchronize();
	S_Device.reset();
	S_CommandQueue->Shutdown();
	S_SwapChain->ShutDown();
	ShutDownCommandList();
	ShutDownPSO();
}

void RenderEngine::Initialize()
{
	auto app	 = Haku::Application::Get();
	auto pWindow = app->GetWindow();
	S_FenceValue = 0;
	S_Device	 = std::make_unique<RenderDevice>();

	// these all need RenderDevice
	S_Fence				 = std::make_unique<Fence>();
	S_SwapChain			 = std::make_unique<SwapChain>();
	S_CommandQueue		 = std::make_unique<CommandQueue>();
	S_Viewport			 = std::make_unique<D3D12_VIEWPORT>();
	S_Viewport->TopLeftX = 0;
	S_Viewport->TopLeftY = 0;
	S_Viewport->Width	 = pWindow->GetWidth();
	S_Viewport->Height	 = pWindow->GetHeight();

	S_ScissorRect = std::make_unique<D3D12_RECT>(0, 0, pWindow->GetWidth(), pWindow->GetHeight());

	InitializeCommandListArrays();
	for (uint64_t i = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; i++)
	{
		S_CPUDescriptorAllocators[i] =
			std::make_unique<DescriptorAllocator>(static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(i));
	}
	S_SwapChain->Init(pWindow);
	InitializePSOWithUISRV();
}

uint64_t RenderEngine::GetFenceValue() noexcept
{
	std::lock_guard<std::mutex> lock(S_mutex);
	return S_FenceValue;
}

uint64_t RenderEngine::FenceValueAdd() noexcept
{
	return S_FenceValue++;
}

Fence* RenderEngine::GetFence() noexcept
{
	return S_Fence.get();
}

//* ()
ID3D12Device* RenderEngine::GetDeviceD3D() noexcept
{
	return S_Device->Get();
}

SwapChain* RenderEngine::GetSwapChain() noexcept
{
	return S_SwapChain.get();
}

CommandQueue* RenderEngine::GetCommandQueue()
{
	return S_CommandQueue.get();
}

void RenderEngine::RecordAndCommandList()
{
	S_CurrentCommandList->TransitionSwapChainPresent();
	S_CurrentCommandList->Close();
}

} // namespace Renderer
} // namespace Haku
