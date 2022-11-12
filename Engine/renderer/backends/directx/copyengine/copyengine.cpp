/*****************************************************************//**
 * \file   copyengine.cpp
 * \brief  
 * 
 * \author June
 * \date   November 2022
 *********************************************************************/
#include "copyengine.hpp"
#include "../directx_backend.hpp"
#include "core/logger.hpp"
#include "../resources.hpp"
#include "memory/hmemory.hpp"

static ID3D12CommandQueue* directcommandqueue = nullptr;
static ID3D12GraphicsCommandList* directcommandlist = nullptr;
static ID3D12CommandAllocator* directallocator = nullptr;
static ID3D12CommandQueue* commandqueue = nullptr;
static ID3D12GraphicsCommandList* commandlist = nullptr;
static ID3D12CommandAllocator* allocator = nullptr;
static ID3D12Fence* fence = nullptr;
static HANDLE event;
static u64 fence_val = 0;

void do_transitions();

i8 initialize_copy_engine(void) 
{
	directx_device* dev = get_device();

	D3D12_COMMAND_QUEUE_DESC queue_desc{};
	queue_desc.Flags = D3D12_COMMAND_QUEUE_FLAG_NONE;
	queue_desc.NodeMask = 0;
	queue_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_HIGH;
	queue_desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;

	HRESULT win32_rv = dev->logical_device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&commandqueue));
	if (S_OK != win32_rv)
	{
		HLCRIT("failed to make copy command queue");
		return H_FAIL;
	}

	queue_desc.Type = D3D12_COMMAND_LIST_TYPE_DIRECT;
	win32_rv = dev->logical_device->CreateCommandQueue(&queue_desc, IID_PPV_ARGS(&directcommandqueue));
	if (S_OK != win32_rv)
	{
		HLCRIT("failed to make util command queue");
		return H_FAIL;
	}

	win32_rv = dev->logical_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY, IID_PPV_ARGS(&allocator));
	if (S_OK != win32_rv)
	{
		HLCRIT("failed to make copy command allocator");
		commandqueue->Release();
		directcommandqueue->Release();
		return H_FAIL;
	}

	win32_rv = dev->logical_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT, IID_PPV_ARGS(&directallocator));
	if (S_OK != win32_rv)
	{
		HLCRIT("failed to make util command allocator");
		commandqueue->Release();
		directcommandqueue->Release();
		allocator->Release();
		return H_FAIL;
	}


	win32_rv = dev->logical_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_COPY, allocator, nullptr, IID_PPV_ARGS(&commandlist));
	if (S_OK != win32_rv)
	{
		HLCRIT("failed to make copy command list");
		commandqueue->Release();
		directcommandqueue->Release();
		allocator->Release();
		directallocator->Release();
		return H_FAIL;
	}

	win32_rv = dev->logical_device->CreateCommandList(0, D3D12_COMMAND_LIST_TYPE_DIRECT, directallocator, nullptr, IID_PPV_ARGS(&directcommandlist));
	if (S_OK != win32_rv)
	{
		HLCRIT("failed to make copy command list");
		commandqueue->Release();
		directcommandqueue->Release();
		allocator->Release();
		directallocator->Release();
		commandlist->Release();
		return H_FAIL;
	}

	win32_rv = dev->logical_device->CreateFence(
		fence_val, D3D12_FENCE_FLAG_NONE, IID_PPV_ARGS(&fence));
	if (S_OK != win32_rv)
	{
		HLCRIT("failed to make copy fence");
		commandqueue->Release();
		directcommandqueue->Release();
		allocator->Release();
		directallocator->Release();
		commandlist->Release();
		directcommandlist->Release();
		return H_FAIL;
	}

	event = CreateEvent(nullptr, FALSE, FALSE, "copy queue event");
	if (nullptr == event)
	{
		HLEMER("synchronizing event failure");
		commandqueue->Release();
		directcommandqueue->Release();
		allocator->Release();
		directallocator->Release();
		commandlist->Release();
		directcommandlist->Release();
		fence->Release();
		return H_FAIL;
	}

	if (nullptr == event)
	{
		HLEMER("synchronizing event failure");
		commandqueue->Release();
		directcommandqueue->Release();
		allocator->Release();
		directallocator->Release();
		commandlist->Release();
		directcommandlist->Release();
		fence->Release();
		return H_FAIL;
	}

	return H_OK;
}

void shutdown_copy_engine(void)
{
	commandqueue->Release();
	directcommandqueue->Release();
	allocator->Release();
	directallocator->Release();
	
	commandlist->Release();
	directcommandlist->Release();
	fence->Release();
	CloseHandle(event);
}


i8 queue_flush(ID3D12CommandQueue* queue)
{
	const UINT64 val = fence_val;
	fence_val++;
	HRESULT		 api_ret_code = queue->Signal(fence, val);
	if (S_OK != api_ret_code)
	{
		HLCRIT("failed to signal to queue");
		return H_FAIL;
	}
	if (fence->GetCompletedValue() < val)
	{
		api_ret_code = fence->SetEventOnCompletion(val, event);
		if (S_OK != api_ret_code)
		{
			HLCRIT("failed to set synchronizing event");
			return H_FAIL;
		}
		WaitForSingleObject(event, INFINITE);
	}
	return H_OK;
}

i8 do_copy(void)
{
	HRESULT win32_rv = commandlist->Close();
	if (S_OK != win32_rv)
	{
		HLCRIT("failed to close copy list");
		return H_FAIL;
	}
	ID3D12CommandList* list [] = { commandlist };

	commandqueue->ExecuteCommandLists(1u, list);
	queue_flush(commandqueue);
	win32_rv = allocator->Reset();
	if (S_OK != win32_rv)
	{
		HLCRIT("failed to reset allocator");
		return H_FAIL;
	}
	win32_rv = commandlist->Reset(allocator, nullptr);
	if (S_OK != win32_rv)
	{
		HLCRIT("failed to reset commandlist ");
		return H_FAIL;
	}

	return H_OK;
}

void copy_buffer(ID3D12Resource* dest, void* data, u64 height, u64 width, u64 depth, D3D12_RESOURCE_DIMENSION dimension)
{
	HASSERT(dest!=nullptr && data != nullptr)
	ID3D12Resource* res = create_resource(height, width, depth, dimension, D3D12_HEAP_TYPE_UPLOAD,
		D3D12_HEAP_FLAG_NONE, DXGI_FORMAT_UNKNOWN, D3D12_RESOURCE_STATE_GENERIC_READ, D3D12_RESOURCE_FLAG_NONE);
	if (nullptr == res)
	{
		HLEMER("failed to create resource");
		return;
	}

	u8* vertex_map = nullptr;
	D3D12_RANGE uprange{};
	HRESULT win32_rv = res->Map(0u, &uprange, (void**)&vertex_map);
	if (S_OK != win32_rv)
	{
		HLCRIT("failed to map value");
		res->Release();
		return;
	}
	hmemory_copy(vertex_map, data, sizeof(hk_vertex) * 4u);
	res->Unmap(0u, nullptr);

	commandlist->CopyResource(dest, res);
	i8 api_rv = do_copy();
	if (H_OK != api_rv)
	{
		HLCRIT("failed to do copy");
	}
	res->Release();
}

void transition_buffer_immediate(ID3D12Resource* dest, D3D12_RESOURCE_STATES before, D3D12_RESOURCE_STATES after, u64 subresource)
{
	D3D12_RESOURCE_BARRIER barrier{};
	barrier.Type = D3D12_RESOURCE_BARRIER_TYPE_TRANSITION;
	barrier.Transition.pResource = dest;
	barrier.Transition.Subresource = subresource;
	barrier.Transition.StateBefore = before;
	barrier.Transition.StateAfter = after;

	directcommandlist->ResourceBarrier(1u, &barrier);
	i8 api_rv = do_copy();
	if (H_OK != api_rv)
	{
		HLCRIT("failed to work");
	}
	do_transitions();
}


void do_transitions()
{
	HRESULT win32_rv = directcommandlist->Close();
	if (S_OK != win32_rv)
	{
		HLCRIT("failed to close copy list");
		return;
	}
	ID3D12CommandList* list[] = { directcommandlist };

	directcommandqueue->ExecuteCommandLists(1u, list);
	queue_flush(directcommandqueue);
	win32_rv = directallocator->Reset();
	if (S_OK != win32_rv)
	{
		HLCRIT("failed to reset allocator");
		return ;
	}
	win32_rv = directcommandlist->Reset(directallocator, nullptr);
	if (S_OK != win32_rv)
	{
		HLCRIT("failed to reset commandlist ");
		return ;
	}

}
