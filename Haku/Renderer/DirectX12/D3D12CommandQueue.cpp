#include "D3D12CommandQueue.hpp"
#include "../../Core/Exceptions.hpp"

namespace Haku
{
namespace Renderer
{
D3D12CommandQueue::D3D12CommandQueue(D3D12RenderDevice& Device)
{
	// Command Queue Creations
	D3D12_COMMAND_QUEUE_DESC Queue_desc{};
	Queue_desc.Type	 = D3D12_COMMAND_LIST_TYPE_DIRECT;
	Queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;

	HAKU_SOK_ASSERT(Device.get()->CreateCommandQueue(&Queue_desc, IID_PPV_ARGS(&m_CommandQueue)))
	for (size_t i = 0; i < FrameCount; i++)
	{
		HAKU_SOK_ASSERT(
			Device.get()->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&m_CommandAllocator[i])))
		HAKU_DXNAME(m_CommandAllocator[i], L"Allocator")
	}

	// Create synchronization objects and wait until assets have been uploaded to the GPU.
	{
		HAKU_SOK_ASSERT(Device.get()->CreateFence(0, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&m_Fence)));
		m_FenceValue = 1;

		// Create an event handle to use for frame synchronization.
		m_FenceEvent = CreateEvent(nullptr, FALSE, FALSE, nullptr);
		if (m_FenceEvent == nullptr)
		{
			HAKU_SOK_ASSERT(HRESULT_FROM_WIN32(GetLastError()));
		}
	}

	HAKU_SOK_ASSERT(Device.get()->CreateCommandList(
		0, D3D12_COMMAND_LIST_TYPE_DIRECT, m_CommandAllocator[0].Get(), nullptr, IID_PPV_ARGS(&m_CommandList)));

	HAKU_DXNAME(m_CommandList, L"Command List")
	HAKU_DXNAME(m_CommandQueue, L"CommandQueue")
}

void D3D12CommandQueue::Reset(ID3D12PipelineState* PipelineState)
{
	ResetCommandAllocator();
	ResetCommandList(PipelineState);
}

void D3D12CommandQueue::Close()
{
	HAKU_SOK_ASSERT(m_CommandList->Close())
}
void D3D12CommandQueue::Execute()
{
	ID3D12CommandList* ppCommandLists[] = { m_CommandList.Get() };
	m_CommandQueue->ExecuteCommandLists(_countof(ppCommandLists), ppCommandLists);
}
void D3D12CommandQueue::ShutDown() noexcept
{
	m_Fence.Reset();
	m_CommandList.Reset();
	m_CommandQueue.Reset();
	for (size_t i = 0; i < FrameCount; i++)
	{
		m_CommandAllocator[i].Reset();
	}
}
void D3D12CommandQueue::Synchronize()
{
	const uint64_t fence_value = m_FenceValue;
	HAKU_SOK_ASSERT(m_CommandQueue->Signal(m_Fence.Get(), fence_value))
	m_FenceValue++;
	if (m_Fence->GetCompletedValue() < fence_value)
	{
		HAKU_SOK_ASSERT(m_Fence->SetEventOnCompletion(fence_value, m_FenceEvent))
		WaitForSingleObject(m_FenceEvent, INFINITE);
	}
}
void D3D12CommandQueue::ResetCommandAllocator()
{
	HAKU_SOK_ASSERT(m_CommandAllocator[m_FenceValue % FrameCount]->Reset())
}
void D3D12CommandQueue::CloseFenceHandle() noexcept
{
	CloseHandle(m_FenceEvent);
}
void D3D12CommandQueue::ResetCommandList(ID3D12PipelineState* PipelineState){
	HAKU_SOK_ASSERT(m_CommandList->Reset(m_CommandAllocator[m_FenceValue % FrameCount].Get(), PipelineState))
}

CommandQueue::CommandQueue(
	RenderDevice&			  device,
	D3D12_COMMAND_LIST_TYPE	  type,
	D3D12_COMMAND_QUEUE_FLAGS flags,
	INT						  priority)
	: m_CurrentIndex(0u)
{
	// Command Queue Creations
	D3D12_COMMAND_QUEUE_DESC Queue_desc{};
	Queue_desc.Type	 = type;
	Queue_desc.Flags = flags;

	HAKU_SOK_ASSERT(device.Get()->CreateCommandQueue(&Queue_desc, IID_PPV_ARGS(&m_CommandQueue)))
	// may expand the naming in the future
	switch (type)
	{
	case D3D12_COMMAND_LIST_TYPE_DIRECT:
	{
		HAKU_DXNAME(m_CommandQueue, L"Direct Command Queue")
		break;
	}
	case D3D12_COMMAND_LIST_TYPE_COMPUTE:
	{
		HAKU_DXNAME(m_CommandQueue, L"Compute Command Queue")
		break;
	}
	case D3D12_COMMAND_LIST_TYPE_COPY:
	{
		HAKU_DXNAME(m_CommandQueue, L"Copy Command Queue")
		break;
	}
	}

	for (size_t i = 0; i < FrameCount; i++)
	{
		HAKU_SOK_ASSERT(device.Get()->CreateCommandAllocator(type, IID_PPV_ARGS(&m_CommandAllocator[i])))
		switch (type)
		{
		case D3D12_COMMAND_LIST_TYPE_DIRECT:
		{
			wchar_t array[100]{};
			_snwprintf(array, 100, L"Direct Command Allocator : %d", i);
			HAKU_DXNAME(m_CommandQueue, array)
			break;
		}
		case D3D12_COMMAND_LIST_TYPE_COMPUTE:
		{
			wchar_t array[100]{};
			_snwprintf(array, 100, L"Compute Command Allocator : %d", i);
			HAKU_DXNAME(m_CommandQueue, array)
			break;
		}
		case D3D12_COMMAND_LIST_TYPE_COPY:
		{
			wchar_t array[100]{};
			_snwprintf(array, 100, L"Copy Command Allocator : %d", i);
			HAKU_DXNAME(m_CommandQueue, array)
			break;
		}
		}
	}
}
CommandQueue::~CommandQueue()
{
	Shutdown();
}
void CommandQueue::Reset()
{
	HAKU_SOK_ASSERT(m_CommandAllocator[m_CurrentIndex % FrameCount]->Reset())
}
void CommandQueue::Shutdown() noexcept
{
	m_CommandQueue->Release();
	for (size_t i = 0; i < FrameCount; i++)
	{
		m_CommandAllocator[i]->Release();
	}
}
} // namespace Renderer
} // namespace Haku
