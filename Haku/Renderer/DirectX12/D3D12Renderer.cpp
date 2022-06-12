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
std::mutex					  RenderEngine::m_mutex;
std::unique_ptr<Fence>		  RenderEngine::m_Fence;
std::unique_ptr<RenderDevice> RenderEngine::m_Device;
std::unique_ptr<SwapChain>	  RenderEngine::m_SwapChain;
std::atomic_uint64_t		  RenderEngine::m_FenceValue;
std::unique_ptr<CommandQueue> RenderEngine::m_CommandQueue;
std::array<std::unique_ptr<DescriptorAllocator>, D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES>
	RenderEngine::m_CPUDescriptorAllocators;

// testing
Haku::Utils::Hk_Dequeue_mt<CommandList*> RenderEngine::m_ExecutableQueue;
ID3D12GraphicsCommandList*				 RenderEngine::CurrentCommandList;
void									 RenderEngine::Render()
{
	RecordAndCommandList();

#if TEST
	CommandList* Commandlist = RequestCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT);
	CurrentCommandList		 = Commandlist->m_CommandList;
	// thhis needs to repaired
	auto resbar = CD3DX12_RESOURCE_BARRIER::Transition(
		m_SwapChain->GetRenderTargetResource(m_SwapChain->GetFrameIndex()),
		D3D12_RESOURCE_STATE_PRESENT,
		D3D12_RESOURCE_STATE_RENDER_TARGET);
	CurrentCommandList->ResourceBarrier(1, &resbar);

	CD3DX12_CPU_DESCRIPTOR_HANDLE rtvHandle(
		m_SwapChain->GetCPUHeapHandleInOffset(D3D12_DESCRIPTOR_HEAP_TYPE_RTV, m_SwapChain->GetFrameIndex()));
	// auto dsvhandle = Heap.GetDSVCPUHandle();

	CurrentCommandList->OMSetRenderTargets(1, &rtvHandle, FALSE, nullptr);
	// clearing back buffer
	const float clearColor[] = { 1.0f, 0.2f, 0.2f, 1.0f };
	CurrentCommandList->ClearRenderTargetView(rtvHandle, clearColor, 0, nullptr);
	auto esbar = CD3DX12_RESOURCE_BARRIER::Transition(
		m_SwapChain->GetRenderTargetResource(m_SwapChain->GetFrameIndex()),
		D3D12_RESOURCE_STATE_RENDER_TARGET,
		D3D12_RESOURCE_STATE_PRESENT);
	CurrentCommandList->ResourceBarrier(1, &esbar);
	CurrentCommandList->Close();
	m_ExecutableQueue.push_back(Commandlist);
	m_CommandQueue->ExecuteTest(m_ExecutableQueue.front()->m_CommandList, 1);
#endif
	// this might be required only when there is a commandlist execution ,,...so bind them together...?
	m_CommandQueue->ExecuteLists();
	m_SwapChain->Present();
	m_CommandQueue->Synchronize();
	m_SwapChain->SetBackBufferIndex();
	ReturnStaleList(m_ExecutableQueue.front());
	m_ExecutableQueue.pop_front();
	RepurposeStaleList();
	// this will be the eventual end game
	// if(!m_CommandQueue->NoCommandListForExecute())
	//{
	//	m_CommandQueue->ExecuteLists();
	//}
}
void RenderEngine::ResizeEvent(uint64_t height, uint64_t width)
{
	m_SwapChain->Resize(height, width);
}

void RenderEngine::ShutDown()
{
	// Synchronzing to make all command list finish execution
	m_CommandQueue->Synchronize();
	m_Device.reset();
	m_CommandQueue->Shutdown();
	m_SwapChain->ShutDown();
	ShutDownCommandlist();
}

void RenderEngine::Initialize()
{
	auto app	 = Haku::Application::Get();
	auto pWindow = app->GetWindow();
	m_FenceValue = 0;
	m_Device	 = std::make_unique<RenderDevice>();

	// these all need RenderDevice
	m_Fence		   = std::make_unique<Fence>();
	m_SwapChain	   = std::make_unique<SwapChain>();
	m_CommandQueue = std::make_unique<CommandQueue>();
	for (uint64_t i = D3D12_DESCRIPTOR_HEAP_TYPE_CBV_SRV_UAV; i < D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES; i++)
	{
		m_CPUDescriptorAllocators[i] =
			std::make_unique<DescriptorAllocator>(static_cast<D3D12_DESCRIPTOR_HEAP_TYPE>(i));
	}
	m_SwapChain->Init(pWindow);
}

uint64_t RenderEngine::GetFenceValue() noexcept
{
	std::lock_guard<std::mutex> lock(m_mutex);
	return m_FenceValue;
}

uint64_t RenderEngine::FenceValueAdd() noexcept
{
	return m_FenceValue++;
}

Fence* RenderEngine::GetFence() noexcept
{
	return m_Fence.get();
}

//* ()
ID3D12Device* RenderEngine::GetDeviceD3D() noexcept
{
	return m_Device->Get();
}

SwapChain* RenderEngine::GetSwapChain() noexcept
{
	return m_SwapChain.get();
}

CommandQueue* RenderEngine::GetCommandQueue()
{
	return m_CommandQueue.get();
}

void RecordAndCommandList()
{
	// CurrentCommandLis
}

} // namespace Renderer
} // namespace Haku
