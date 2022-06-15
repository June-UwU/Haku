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

// TEST VALUES
const uint32_t TextureHeight	= 256u;
const uint32_t TextureWidth		= 256u;
const uint32_t TexturePixelSize = 4u;

namespace Haku
{
namespace Renderer
{
std::mutex					  RenderEngine::S_mutex;
std::unique_ptr<Fence>		  RenderEngine::S_Fence;
std::unique_ptr<RenderDevice> RenderEngine::S_Device;
std::unique_ptr<SwapChain>	  RenderEngine::S_SwapChain;
std::atomic_uint64_t		  RenderEngine::S_FenceValue;
std::unique_ptr<CommandQueue> RenderEngine::S_CommandQueue;
std::array<std::unique_ptr<DescriptorAllocator>, D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES>
	RenderEngine::S_CPUDescriptorAllocators;

// testing
CommandList* RenderEngine::S_CurrentCommandList;
void		 RenderEngine::EndScene()
{
	S_CurrentCommandList = nullptr;
	ResetStaleList();//reuse the list
};
void RenderEngine::BeginScene()
{
	// getting a command list and then resetting it for recording
	S_CurrentCommandList = RequestCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT);
	S_CurrentCommandList->Reset();
};
void RenderEngine::Render()
{
	BeginScene();
	RecordAndCommandList();

#if TEST
	auto* CurrentCommandList = S_CurrentCommandList->m_CommandList;
	// thhis needs to repaired
	auto resbar = CD3DX12_RESOURCE_BARRIER::Transition(
		S_SwapChain->GetRenderTargetResource(S_SwapChain->GetFrameIndex()),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET);
	CurrentCommandList->ResourceBarrier(1, &resbar);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
		S_SwapChain->GetCPUHeapHandleInOffset(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, S_SwapChain->GetFrameIndex()));
	// auto dsvhandle = Heap.GetDSVCPUHandle();

	CurrentCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
	// clearing back buffer
	const float clearColor[] = { 1.0f, 0.2f, 0.2f, 1.0f };
	CurrentCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	auto esbar = CD3DX12_RESOURCE_BARRIER::Transition(
		S_SwapChain->GetRenderTargetResource(S_SwapChain->GetFrameIndex()),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT);
	CurrentCommandList->ResourceBarrier(1, &esbar);
	HAKU_SOK_ASSERT(CurrentCommandList->Close())
#endif
	// this might be required only when there is a commandlist execution ,,...so bind them together...?
	// S_CommandQueue->Execute(CurrentCommandList, 1);
	S_CommandQueue->AddListAndExecute(S_CurrentCommandList);
	S_SwapChain->Present();
	S_CommandQueue->Synchronize();
	S_SwapChain->SetBackBufferIndex();
	// this will be the eventual end game
	// if(!m_CommandQueue->NoCommandListForExecute())
	//{
	//	m_CommandQueue->ExecuteLists();
	//}
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
}

void RenderEngine::Initialize()
{
	auto app	 = Haku::Application::Get();
	auto pWindow = app->GetWindow();
	S_FenceValue = 0;
	S_Device	 = std::make_unique<RenderDevice>();

	// these all need RenderDevice
	S_Fence		   = std::make_unique<Fence>();
	S_SwapChain	   = std::make_unique<SwapChain>();
	S_CommandQueue = std::make_unique<CommandQueue>();
	for (uint64_t i = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; i++)
	{
		S_CPUDescriptorAllocators[i] =
			std::make_unique<DescriptorAllocator>(static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(i));
	}
	S_SwapChain->Init(pWindow);
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

void RecordAndCommandList()
{
	// CurrentCommandLis
}

} // namespace Renderer
} // namespace Haku
