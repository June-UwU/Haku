// TODO : Create and make the texture class work then convert this into a Interface that provides functions for
// everything

#pragma once
#include <mutex>
#include <atomic>
#include "macros.hpp"
#include "hakupch.hpp"
#include "D3D12Fence.hpp"
#include "Utils/Queue_MT.hpp"
#include "D3D12SwapChain.hpp"
#include "Core/Exceptions.hpp"
#include "D3D12CommandList.hpp"
#include "D3D12RenderDevice.hpp"
#include "D3D12CommandQueue.hpp"
#include "D3D12DescriptorAllocator.hpp"
#include "Utils/Dequeue.hpp"

// TODO : make the gpu allocated resource and the virtual address to be on position and

#include "Platform/Windows/MainWindow.hpp"

#include "D3D12RootSignature.hpp"
#include "D3D12PipelineState.hpp"
// NOTE : this will be part of the ECS..?
#include "D3D12Resources.hpp"
#include "PerspectiveCamera.hpp"

namespace Haku
{
namespace Renderer
{
void RecordAndCommandList();
class RenderEngine
{
public:
	static void			 Render();
	static void			 ShutDown();
	static void			 EndScene(){};
	static void			 Initialize();
	static void			 BeginScene(){};
	static CommandQueue* GetCommandQueue();
	static Fence*		 GetFence() noexcept;
	static ID3D12Device* GetDeviceD3D() noexcept;
	static SwapChain*	 GetSwapChain() noexcept;
	static uint64_t		 GetFenceValue() noexcept;
	static uint64_t		 FenceValueAdd() noexcept;
	static void			 ResizeEvent(uint64_t height, uint64_t width);

private:
	static std::mutex					 m_mutex;
	static std::unique_ptr<Fence>		 m_Fence;
	static std::unique_ptr<RenderDevice> m_Device;
	static std::unique_ptr<SwapChain>	 m_SwapChain;
	static std::atomic_uint64_t			 m_FenceValue;
	static std::unique_ptr<CommandQueue> m_CommandQueue;
	static std::array<std::unique_ptr<DescriptorAllocator>, D3D12_DESCRIPTOR_HEAP_TYPE_NUM_TYPES>
		m_CPUDescriptorAllocators;

	// TESTING THESE
	static Haku::Utils::Hk_Dequeue_mt<CommandList*> m_ExecutableQueue;
	static ID3D12GraphicsCommandList*				CurrentCommandList;
};

} // namespace Renderer
} // namespace Haku
