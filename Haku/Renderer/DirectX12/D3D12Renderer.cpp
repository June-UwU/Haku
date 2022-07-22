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
std::unique_ptr<UploadBuffer> RenderEngine::S_UploadBuffer;
// test
//D3D12_VERTEX_BUFFER_VIEW RenderEngine::T_View;
//ID3D12Resource*			 RenderEngine::T_VertexBuffer;

std::unique_ptr<VertexBuffer> RenderEngine::S_VertexBuffer;

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
	auto pso = GetPSO(0);
	S_CurrentCommandList->SetPipelineState(pso);
	S_CurrentCommandList->m_CommandList->SetGraphicsRootSignature(pso->GetRootSignature());
	S_CurrentCommandList->m_CommandList->IASetVertexBuffers(0, 1, S_VertexBuffer->GetView());
	S_CurrentCommandList->m_CommandList->DrawInstanced(3, 1, 0, 0);
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
	S_UploadBuffer		 = std::make_unique<UploadBuffer>();
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

	// test
	auto commandlist = RequestCommandList(D3D12_COMMAND_LIST_TYPE_DIRECT);
	S_CommandQueue->ExecuteLists(commandlist);
	S_CommandQueue->Synchronize();
	commandlist->Reset();

	VertexData bufferdata[] = { { { 0.0f, 0.25f, 0.0f }, { 0.2f, 0.0f, 0.0f, 0.4f } },
								{ { 0.25f, -0.25f, 0.0f }, { 0.0f, 0.3f, 0.0f, 0.4f } },
								{ { -0.25f, -0.25f, 0.0f }, { 0.0f, 0.0f, 0.1f, 0.4f } } };
	size_t	   buffersize	= 3u * sizeof(VertexData);

	S_UploadBuffer->SubmitVertex(bufferdata, 3);
	S_VertexBuffer = S_UploadBuffer->MakeVertexBuffer();
	S_UploadBuffer->TransitionVertexBuffers(commandlist);
	//auto			Device = GetDeviceD3D();
	//ID3D12Resource* ImmediateBuffer{};
	//auto			upheapprop = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_UPLOAD);
	//auto			heapprop   = CD3DX12_HEAP_PROPERTIES(D3D12_HEAP_TYPE_DEFAULT);
	//auto			resdesc	   = CD3DX12_RESOURCE_DESC::Buffer(buffersize, D3D12_RESOURCE_FLAG_NONE);
	//auto			sok		   = Device->CreateCommittedResource(
	//	  &heapprop,
	//	  D3D12_HEAP_FLAG_NONE,
	//	  &resdesc,
	//	  D3D12_RESOURCE_STATE_COPY_DEST,
	//	  nullptr,
	//	  __uuidof(ID3D12Resource),
	//	  reinterpret_cast<void**>(&T_VertexBuffer));
	//HAKU_SOK_ASSERT(sok);
	//
	//sok = Device->CreateCommittedResource(
	//	&upheapprop,
	//	D3D12_HEAP_FLAG_NONE,
	//	&resdesc,
	//	D3D12_RESOURCE_STATE_GENERIC_READ,
	//	nullptr,
	//	__uuidof(ID3D12Resource),
	//	reinterpret_cast<void**>(&ImmediateBuffer));
	//
	//T_View.BufferLocation = T_VertexBuffer->GetGPUVirtualAddress();
	//T_View.SizeInBytes	  = 3 * sizeof(VertexData);
	//T_View.StrideInBytes  = sizeof(VertexData);
	//uint8_t*	  ptr;
	//CD3DX12_RANGE readrange(0, 0);
	//HAKU_SOK_ASSERT(ImmediateBuffer->Map(0, &readrange, reinterpret_cast<void**>(&ptr)))
	//memcpy(ptr, bufferdata, buffersize);
	//ImmediateBuffer->Unmap(0, nullptr);
	//
	//D3D12_SUBRESOURCE_DATA subresourcedata{};
	//subresourcedata.pData	   = ptr;
	//subresourcedata.SlicePitch = buffersize;
	//subresourcedata.RowPitch   = buffersize;
	//
	//UpdateSubresources(commandlist->m_CommandList, T_VertexBuffer, ImmediateBuffer, 0, 0, 1, &subresourcedata);
	//CD3DX12_RESOURCE_BARRIER barrier = CD3DX12_RESOURCE_BARRIER::Transition(
	//	T_VertexBuffer, D3D12_RESOURCE_STATE_COPY_DEST, D3D12_RESOURCE_STATE_VERTEX_AND_CONSTANT_BUFFER);
	//commandlist->m_CommandList->ResourceBarrier(1, &barrier);

	commandlist->Close();
	S_CommandQueue->ExecuteLists(commandlist);
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
