#include "D3D12Renderer.hpp"
#include "Core/Exceptions.hpp"
#include "D3D12CommandList.hpp"
#include "D3D12CommandQueue.hpp"
namespace Haku
{
namespace Renderer
{
static Utils::HK_Queue_mt<CommandList*> CommandQueue::m_ReadyCmdList;

CommandQueue::CommandQueue(D3D12_COMMAND_QUEUE_FLAGS flags, INT priority)
{
	auto Device = RenderEngine::GetDeviceD3D();
	// Command Queue Creations
	for (size_t i = 0; i < m_CommandQueueArray.size(); i++)
	{
		D3D12_COMMAND_QUEUE_DESC Queue_desc{};
		Queue_desc.Flags = flags;
		Queue_desc.Type	 = static_cast<D3D12_COMMAND_LIST_TYPE>(i);

		HAKU_SOK_ASSERT(Device->CreateCommandQueue(
			&Queue_desc, __uuidof(ID3D12CommandQueue), reinterpret_cast<void**>(&m_CommandQueueArray[i])))
	}
	///// may expand the naming in the future
	/// switch (type)
	///{
	/// case D3D12_COMMAND_LIST_TYPE_DIRECT:
	///{
	///	HAKU_DXNAME(m_CommandQueue, L"Direct Command Queue")
	///	break;
	///}
	/// case D3D12_COMMAND_LIST_TYPE_COMPUTE:
	///{
	///	HAKU_DXNAME(m_CommandQueue, L"Compute Command Queue")
	///	break;
	///}
	/// case D3D12_COMMAND_LIST_TYPE_COPY:
	///{
	///	HAKU_DXNAME(m_CommandQueue, L"Copy Command Queue")
	///	break;
	///}
	///}

	// for (size_t i = 0; i < FrameCount; i++)
	//{
	//	HAKU_SOK_ASSERT(device.Get()->CreateCommandAllocator(type, IID_PPV_ARGS(&m_CommandAllocator[i])))
	//	switch (type)
	//	{
	//	case D3D12_COMMAND_LIST_TYPE_DIRECT:
	//	{
	//		wchar_t array[100]{};
	//		_snwprintf(array, 100, L"Direct Command Allocator : %d", i);
	//		HAKU_DXNAME(m_CommandQueue, array)
	//		break;
	//	}
	//	case D3D12_COMMAND_LIST_TYPE_COMPUTE:
	//	{
	//		wchar_t array[100]{};
	//		_snwprintf(array, 100, L"Compute Command Allocator : %d", i);
	//		HAKU_DXNAME(m_CommandQueue, array)
	//		break;
	//	}
	//	case D3D12_COMMAND_LIST_TYPE_COPY:
	//	{
	//		wchar_t array[100]{};
	//		_snwprintf(array, 100, L"Copy Command Allocator : %d", i);
	//		HAKU_DXNAME(m_CommandQueue, array)
	//		break;
	//	}
	//	}
	//}
}
CommandQueue::~CommandQueue()
{
	Shutdown();
}

void CommandQueue::Shutdown() noexcept
{
	for (auto* ptr : m_CommandQueueArray)
	{
		ptr->Release();
	}
}
void CommandQueue::ExecuteLists() noexcept
{
	if (!m_ReadyCmdList.empty())
	{
		auto Current = m_ReadyCmdList.front();
		//
		// Current->ResolveResourceStates();
		// auto Queue = Get(Current->GetType());
		// Queue->ExecuteCommandLists(Current->GetListSize(),Current->GetList());
		// Synchronize();
		// RenderEngine::ReturnStaleCommandList(Current);
		// m_CommandQueueArray.pop();
	}
}
void CommandQueue::EmplaceCommandList(CommandList* ptr) noexcept
{
	m_ReadyCmdList.push(ptr);
}
void CommandQueue::Synchronize()
{
	//	// Get the device and synchronizing value
	auto fenceptr	= RenderEngine::GetFence();
	auto fence		= fenceptr->Get();
	auto fenceevent = fenceptr->GetEvent();
	auto fencevalue = RenderEngine::FenceValueAdd();
	auto Swapchain	= RenderEngine::GetSwapChain(); // this is taken to update the backbuffer index (m_FrameIndex)
	// auto		   DirectQueue = RenderEngine::GetCommandQueue();
	const uint64_t fence_value = fencevalue;
	HAKU_SOK_ASSERT(m_CommandQueueArray[D3D12_COMMAND_LIST_TYPE_DIRECT]->Signal(fence, fence_value))
	// updating fence value
	RenderEngine::FenceValueAdd();
	if (fence->GetCompletedValue() < fence_value)
	{
		HAKU_SOK_ASSERT(fence->SetEventOnCompletion(fence_value, fenceevent))
		WaitForSingleObject(fenceevent, INFINITE);
	}
	Swapchain->SetBackBufferIndex();
}
} // namespace Renderer
} // namespace Haku
