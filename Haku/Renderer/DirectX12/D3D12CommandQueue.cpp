#include "D3D12Renderer.hpp"
#include "Core/Exceptions.hpp"
#include "D3D12CommandList.hpp"
#include "D3D12CommandQueue.hpp"
namespace Haku
{
namespace Renderer
{
CommandQueue::CommandQueue(D3D12_COMMAND_QUEUE_FLAGS flags, INT priority)
{
	auto Device = RenderEngine::GetDeviceD3D();
	// Command Queue Creations

	D3D12_COMMAND_QUEUE_DESC Queue_desc{};
	Queue_desc.Flags = flags;
	Queue_desc.Type	 = D3D12_COMMAND_LIST_TYPE_DIRECT;

	HAKU_SOK_ASSERT(Device->CreateCommandQueue(
		&Queue_desc,
		__uuidof(ID3D12CommandQueue),
		reinterpret_cast<void**>(&m_CommandQueueArray[D3D12_COMMAND_LIST_TYPE_DIRECT])))
	HAKU_DXNAME(m_CommandQueueArray[0], L"Direct Command Queue");
	Queue_desc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;

	HAKU_SOK_ASSERT(Device->CreateCommandQueue(
		&Queue_desc, __uuidof(ID3D12CommandQueue), reinterpret_cast<void**>(&m_CommandQueueArray[1])))
	HAKU_DXNAME(m_CommandQueueArray[1], L"Compute Command Queue");

	Queue_desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;

	HAKU_SOK_ASSERT(Device->CreateCommandQueue(
		&Queue_desc, __uuidof(ID3D12CommandQueue), reinterpret_cast<void**>(&m_CommandQueueArray[2])))
	HAKU_DXNAME(m_CommandQueueArray[2], L"Copy Command Queue");
}
CommandQueue::~CommandQueue()
{
	// Shutdown();
}

void CommandQueue::Shutdown() noexcept
{
	for (auto* ptr : m_CommandQueueArray)
	{
		ptr->Release();
	}
}

void CommandQueue::ExecuteLists(CommandList* list)
{
	m_CommandQueueArray[0]->ExecuteCommandLists(1, reinterpret_cast<ID3D12CommandList*const*>(&list->m_CommandList));
}

ID3D12CommandQueue* CommandQueue::Get(D3D12_COMMAND_LIST_TYPE type)
{
	ID3D12CommandQueue* ret_val = nullptr;
	switch (type)
	{
	case D3D12_COMMAND_LIST_TYPE_DIRECT:
	{
		ret_val = m_CommandQueueArray[0];
		break;
	}
	case D3D12_COMMAND_LIST_TYPE_COMPUTE:
	{
		ret_val = m_CommandQueueArray[1];
		break;
	}
	case D3D12_COMMAND_LIST_TYPE_COPY:
	{
		ret_val = m_CommandQueueArray[2];
		break;
	}
	default:
	{
		HAKU_LOG_CRIT("Unknown Type of Command Queue", type);
		HAKU_THROW("Unknown Type of Command Queue");
		break;
	}
	}
	return ret_val;
}

void CommandQueue::Synchronize()
{
	//	// Get the device and synchronizing value
	auto fenceptr	= RenderEngine::GetFence();
	auto fence		= fenceptr->Get();
	auto fenceevent = fenceptr->GetEvent();
	auto fencevalue = RenderEngine::FenceValueAdd();
	// auto Swapchain	= RenderEngine::GetSwapChain(); // this is taken to update the backbuffer index (m_FrameIndex)
	// auto		   DirectQueue = RenderEngine::GetCommandQueue();
	const uint64_t fence_value = fencevalue;
	HAKU_SOK_ASSERT(m_CommandQueueArray[D3D12_COMMAND_LIST_TYPE_DIRECT]->Signal(fence, fence_value))
	// updating fence value
	// RenderEngine::FenceValueAdd();
	if (fence->GetCompletedValue() < fence_value)
	{
		HAKU_SOK_ASSERT(fence->SetEventOnCompletion(fence_value, fenceevent))
		WaitForSingleObject(fenceevent, INFINITE);
	}
}
} // namespace Renderer
} // namespace Haku
