// TODO : Create and make the texture class work then convert this into a Interface that provides functions for
// everything

#pragma once
#include "macros.hpp"
#include "hakupch.hpp"
#include "D3D12Fence.hpp"
#include "D3D12SwapChain.hpp"
#include "Core/Exceptions.hpp"
#include "D3D12CommandList.hpp"
#include "D3D12RenderDevice.hpp"
#include "D3D12CommandQueue.hpp"
#include "D3D12DescriptorAllocator.hpp"
#include "Platform/Windows/MainWindow.hpp"

// TODO : make the gpu allocated resource and the virtual address to be on position and

// NOTE : this will be part of the ECS..?

namespace Haku
{
namespace Renderer
{
class RenderEngine
{
public:
	static void			 Render();
	static void			 ShutDown();
	static void			 EndScene();
	static void			 Initialize();
	static void			 BeginScene();
	static CommandQueue* GetCommandQueue();
	static Fence*		 GetFence() noexcept;
	static void			 RecordAndCommandList();
	static ID3D12Device* GetDeviceD3D() noexcept;
	static SwapChain*	 GetSwapChain() noexcept;
	static uint64_t		 GetFenceValue() noexcept;
	static uint64_t		 FenceValueAdd() noexcept;
	static void			 ResizeEvent(uint64_t height, uint64_t width);

private:
	static std::mutex					   S_mutex;
	static std::unique_ptr<Fence>		   S_Fence;
	static std::unique_ptr<RenderDevice>   S_Device;
	static std::unique_ptr<D3D12_VIEWPORT> S_Viewport;
	static std::unique_ptr<SwapChain>	   S_SwapChain;
	static std::atomic_uint64_t			   S_FenceValue;
	static std::unique_ptr<D3D12_RECT>	   S_ScissorRect;
	static std::unique_ptr<CommandQueue>   S_CommandQueue;
	static CommandList*					   S_CurrentCommandList;
	static std::array<std::unique_ptr<DescriptorAllocator>, D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES>
		S_CPUDescriptorAllocators;
};

} // namespace Renderer
} // namespace Haku
