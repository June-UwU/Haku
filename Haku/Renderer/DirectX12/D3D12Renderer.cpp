#include "directx/d3d12.h"
#include "d3dcompiler.h"
#include "Core/UILayer.hpp"
#include "D3D12Renderer.hpp"
#include "Core/Exceptions.hpp"
#include "Core/Application.hpp"
#include "..\..\imgui\backends\imgui_impl_dx12.h"

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
std::unique_ptr<Fence>				 RenderEngine::m_Fence;
std::unique_ptr<RenderDevice>		 RenderEngine::m_Device;
std::unique_ptr<SwapChain>			 RenderEngine::m_SwapChain;
std::atomic_uint64_t				 RenderEngine::m_FenceValue;
Utils::HK_Queue_mt<CommandList*>	 RenderEngine::m_FreeCmdList;
std::unique_ptr<CommandQueue>		 RenderEngine::m_CommandQueue;
std::unique_ptr<DescriptorAllocator> RenderEngine::m_CPUDescriptorAllocators[D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES];

void RenderEngine::Render()
{
	if (m_FreeCmdList.empty())
	{
		m_FreeCmdList.push(CreateCommandList());
	}
}

void RenderEngine::ShutDown()
{
	m_Device.reset();
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
	return m_FenceValue;
}

uint64_t RenderEngine::FenceValueAdd() noexcept
{
	return m_FenceValue++;
}

void RenderEngine::ReturnStaleCommandList(CommandList* ptr)
{
	m_FreeCmdList.push(ptr);
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

} // namespace Renderer
} // namespace Haku
