#include "core/logger.hpp"
#include "command_context.hpp"
#include "command_buffer_pool.hpp"


typedef enum
{
	event_fail,
	fence_fail,
	copy_queue_failure,
	compute_queue_fail,
	render_queue_fail
}command_context_fails;

// TODO : this is not a usable function that is usable right now, there are thing need to be done here
i8 next_frame_synchronization(directx_queue* queue, directx_commandlist* commandlist);
i8 command_context_fail_handler(directx_queue* queue, command_context_fails fail_code);

i8 command_context_initialize(directx_context* context)
{

	directx_queue* queue = &context->queue;
	queue->fence_val     = 0;
	HLINFO("command queue initialization");
	i8 ret_code		= H_OK;
	HRESULT	api_ret_code	= S_OK;

	D3D12_COMMAND_QUEUE_DESC q_desc{};
	q_desc.Type	= D3D12_COMMAND_LIST_TYPE_DIRECT;
	q_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	q_desc.Flags	= D3D12_COMMAND_QUEUE_FLAG_NONE;
	q_desc.NodeMask	= 0;

	// creating render queue
	api_ret_code	= context->logical_device->CreateCommandQueue(&q_desc,__uuidof(ID3D12CommandQueue),(void**)&queue->directx_queue[HK_COMMAND_RENDER]);
	if(S_OK != api_ret_code)
	{
		HLEMER("render queue failure");
		ret_code= command_context_fail_handler(queue, render_queue_fail);
		return ret_code;
	}
	HLINFO("render queue initailized");
	FRIENDLY_NAME(queue->directx_queue[HK_COMMAND_RENDER], L"DirectX Render Queue");



	// creating compute queue
	q_desc.Type	= D3D12_COMMAND_LIST_TYPE_COMPUTE;

	api_ret_code	= context->logical_device->CreateCommandQueue(&q_desc,__uuidof(ID3D12CommandQueue),(void**)&queue->directx_queue[HK_COMMAND_COMPUTE]);
	if(S_OK != api_ret_code)
	{
		HLEMER("compute queue creation failure");
		ret_code = command_context_fail_handler(queue, compute_queue_fail);
		return ret_code;
	}
	HLINFO("compute queue initailized");
	FRIENDLY_NAME(queue->directx_queue[HK_COMMAND_COMPUTE], L"DirectX Compute Queue");

	
	// creating copy queue
	q_desc.Type	= D3D12_COMMAND_LIST_TYPE_COPY;

	api_ret_code	= context->logical_device->CreateCommandQueue(&q_desc,__uuidof(ID3D12CommandQueue),(void**)&queue->directx_queue[HK_COMMAND_COPY]);
	if(S_OK != api_ret_code)
	{
		HLEMER("copy queue failure");
		ret_code = command_context_fail_handler(queue, copy_queue_failure);
		return ret_code;
	}
	HLINFO("copy queue initailized");
	FRIENDLY_NAME(queue->directx_queue[HK_COMMAND_COPY], L"DirectX Copy Queue");

	api_ret_code = context->logical_device->CreateFence(queue->fence_val, D3D12_FENCE_FLAG_NONE,
		__uuidof(ID3D12Fence), (void**)&queue->fence);
	if (S_OK != api_ret_code)
	{
		HLEMER("synchronizing fence failure");
		ret_code = command_context_fail_handler(queue, fence_fail);
		return ret_code;
	}
	HLINFO("synchronizing fence is initialized");
	FRIENDLY_NAME(queue->fence, L"command queue fence");

	queue->event = CreateEvent(nullptr, FALSE, FALSE, "queue event");
	if (nullptr == queue->event)
	{
		HLEMER("synchronizing event failure");
		ret_code = command_context_fail_handler(queue, event_fail);
		return ret_code;
	}
	HLINFO("synchronizing event is initialized");

	return ret_code;
}

void command_context_shutdown(directx_queue* queue)
{
	HLINFO("command queue shutdown");
	for(u64 i = 0; i< HK_COMMAND_MAX; i++)
	{
		queue->directx_queue[i]->Release();
	}
	HLINFO("releasing fence ");
	queue->fence->Release();
	
	HLINFO("releasing event");
	CloseHandle(queue->event);
}

void execute_command(directx_context* context, directx_commandlist* commandlist)
{
	ID3D12CommandList* list[] = { commandlist->commandlist };
	context->queue.directx_queue[commandlist->type]->ExecuteCommandLists(1, list);
	commandlist->seeded_allocator->state = COMMAND_BUFFER_STATE_SUBMITTED;
	next_frame_synchronization(&context->queue,commandlist);
}

i8 prepare_commandlist_record(directx_commandlist* commandlist)
{
	HRESULT api_ret_code = S_OK;

	directx_allocator* alloc_ptr = request_command_buffer(commandlist->type);

	if (nullptr == alloc_ptr)
	{
		HLCRIT("command buffer pool returned nullptr for reset");
		return H_FAIL;
	}
	alloc_ptr->state = COMMAND_BUFFER_STATE_RECORDING;

	commandlist->seeded_allocator = alloc_ptr;

	api_ret_code = commandlist->commandlist->Reset(alloc_ptr->allocator, nullptr);

	if (S_OK != api_ret_code)
	{
		HLEMER("commandlist result returned an error");
		return H_FAIL;
	}

	return H_OK;
}

i8 end_commandlist_record(directx_commandlist* commandlist)
{
	HRESULT api_ret_code = commandlist->commandlist->Close();
	if (S_OK != api_ret_code)
	{
		HLCRIT("command list failed to close");
		return H_FAIL;
	}
	commandlist->seeded_allocator->state = COMMAND_BUFFER_STATE_RECORDING_ENDED;
	return H_OK;
}

i8 next_frame_synchronization(directx_queue* queue,directx_commandlist* commandlist)
{
	i8 ret_code = H_OK;

	HRESULT api_ret_code = H_OK;

	queue->fence_val++;
	api_ret_code = queue->directx_queue[commandlist->type]->Signal(queue->fence, queue->fence_val);
	if(H_OK != api_ret_code)
	{
		HLCRIT("signal failed");
		ret_code = H_FAIL;
		return ret_code;
	}
	commandlist->seeded_allocator->fence_val = queue->fence_val;

	u64 completed_val = queue->fence->GetCompletedValue();

	reintroduce_allocator(completed_val);

	return ret_code;
}

i8 command_context_fail_handler(directx_queue* queue,command_context_fails fail_code)
{
	switch (fail_code)
	{
	case event_fail:
		queue->fence->Release();
	case fence_fail:
		queue->directx_queue[HK_COMMAND_COPY]->Release();
	case copy_queue_failure:
		queue->directx_queue[HK_COMMAND_COMPUTE]->Release();
	case compute_queue_fail:
		queue->directx_queue[HK_COMMAND_RENDER]->Release();
	case render_queue_fail:
		break;
	}
	return H_FAIL;
}
