#include "buffer.hpp"
#include "core/logger.hpp"
#include "command_context.hpp"
#include "command_buffer_pool.hpp"

/** enum to poiut out the context creation failure that occured to handle it properly */
typedef enum command_context_fails
{
	/** event creation failure */
	event_fail,

	/** fence creation failure */
	fence_fail,

	/** copy queue creation failure */
	copy_queue_failure,

	/** compute context creation failure */
	compute_queue_fail,

	/** render context creation failure */
	render_queue_fail
} command_context_fails;

/**
 * internal routine to do a full queue flush.
 * 
 * \param queue queue to flush
 * \param type type of queue to flush
 * \return H_OK on sucess
 */
i8 queue_flush(directx_queue* queue, queue_type type);

/**
 * internal method to do a next frame flush.
 * 
 * \param queue queue to flush
 * \param commandlist that is used to determine the type
 * \return H_OK on sucess
 */
i8 next_frame_synchronization(directx_queue* queue, directx_cc* commandlist);

/**
 * command context failhandler.
 * 
 * \param queue queue to handle 
 * \param fail_code code to point failure point
 * \return H_FAIL
 */
i8 command_context_fail_handler(directx_queue* queue, command_context_fails fail_code);

i8 command_context_initialize(directx_context* context)
{
	directx_queue* queue = &context->queue;
	directx_device* device = &context->device;
	queue->fence_val	 = 0;
	HLINFO("command queue initialization");
	i8		ret_code	 = H_OK;
	HRESULT api_ret_code = S_OK;

	D3D12_COMMAND_QUEUE_DESC q_desc{};
	q_desc.Type		= D3D12_COMMAND_LIST_TYPE_DIRECT;
	q_desc.Priority = D3D12_COMMAND_QUEUE_PRIORITY_NORMAL;
	q_desc.Flags	= D3D12_COMMAND_QUEUE_FLAG_NONE;
	q_desc.NodeMask = 0;

	// creating render queue
	api_ret_code = device->logical_device->CreateCommandQueue(
		&q_desc, __uuidof(ID3D12CommandQueue), (void**)&queue->directx_queue[HK_COMMAND_RENDER]);
	if (S_OK != api_ret_code)
	{
		HLEMER("render queue failure");
		ret_code = command_context_fail_handler(queue, render_queue_fail);
		return ret_code;
	}
	HLINFO("render queue initailized");
	FRIENDLY_NAME(queue->directx_queue[HK_COMMAND_RENDER], L"DirectX Render Queue");

	// creating compute queue
	q_desc.Type = D3D12_COMMAND_LIST_TYPE_COMPUTE;

	api_ret_code = device->logical_device->CreateCommandQueue(
		&q_desc, __uuidof(ID3D12CommandQueue), (void**)&queue->directx_queue[HK_COMMAND_COMPUTE]);
	if (S_OK != api_ret_code)
	{
		HLEMER("compute queue creation failure");
		ret_code = command_context_fail_handler(queue, compute_queue_fail);
		return ret_code;
	}
	HLINFO("compute queue initailized");
	FRIENDLY_NAME(queue->directx_queue[HK_COMMAND_COMPUTE], L"DirectX Compute Queue");

	// creating copy queue
	q_desc.Type = D3D12_COMMAND_LIST_TYPE_COPY;

	api_ret_code = device->logical_device->CreateCommandQueue(
		&q_desc, __uuidof(ID3D12CommandQueue), (void**)&queue->directx_queue[HK_COMMAND_COPY]);
	if (S_OK != api_ret_code)
	{
		HLEMER("copy queue failure");
		ret_code = command_context_fail_handler(queue, copy_queue_failure);
		return ret_code;
	}
	HLINFO("copy queue initailized");
	FRIENDLY_NAME(queue->directx_queue[HK_COMMAND_COPY], L"DirectX Copy Queue");

	api_ret_code = device->logical_device->CreateFence(
		queue->fence_val, D3D12_FENCE_FLAG_NONE, __uuidof(ID3D12Fence), (void**)&queue->fence);
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
	for (u64 i = 0; i < HK_COMMAND_MAX; i++)
	{
		queue->directx_queue[i]->Release();
	}
	HLINFO("releasing fence ");
	queue->fence->Release();

	HLINFO("releasing event");
	CloseHandle(queue->event);
}

void execute_command(directx_context* context, directx_cc* commandlist)
{
	ID3D12CommandList* list[] = { commandlist->commandlist };
	context->queue.directx_queue[commandlist->type]->ExecuteCommandLists(1, list);
	commandlist->state = COMMAND_BUFFER_STATE_SUBMITTED;
}


i8 end_commandlist_record(directx_cc* commandlist)
{
	HRESULT api_ret_code = commandlist->commandlist->Close();
	if (S_OK != api_ret_code)
	{
		HLCRIT("command list failed to close");
		return H_FAIL;
	}
	commandlist->state = COMMAND_BUFFER_STATE_RECORDING_ENDED;
	return H_OK;
}

i8 full_gpu_flush(directx_queue* queue, queue_type type)
{
	i8 ret_code = H_OK;
	HLWARN("FULL  GPU FLUSH : if this is called on frame render this is a serious bug..!!");

	ret_code = queue_flush(queue, type);
	if (H_OK != ret_code)
	{
		return H_FAIL;
	}

	return ret_code;
}

i8 next_frame_synchronization(directx_queue* queue, directx_cc* commandlist)
{
	i8 ret_code = H_OK;

	HRESULT api_ret_code = H_OK;

	queue->fence_val++;
	api_ret_code = queue->directx_queue[commandlist->type]->Signal(queue->fence, queue->fence_val);
	if (S_OK != api_ret_code)
	{
		HLCRIT("signal failed");
		ret_code = H_FAIL;
		return ret_code;
	}
	commandlist->fence_val = queue->fence_val;

	u64 completed_val = queue->fence->GetCompletedValue();
	if (completed_val > 0)
	{
		completed_val--;
	}
	reintroduce_dxcc(completed_val);
	reclaim_buffer(completed_val);


	return ret_code;
}

i8 queue_flush(directx_queue* queue, queue_type type)
{
	const UINT64 fence		  = queue->fence_val++;
	HRESULT		 api_ret_code = queue->directx_queue[HK_COMMAND_RENDER]->Signal(queue->fence, fence);
	if (S_OK != api_ret_code)
	{
		HLCRIT("failed to signal to queue");
		return H_FAIL;
	}
	if (queue->fence->GetCompletedValue() < fence)
	{
		api_ret_code = queue->fence->SetEventOnCompletion(fence, queue->event);
		if (S_OK != api_ret_code)
		{
			HLCRIT("failed to set synchronizing event");
			return H_FAIL;
		}
		WaitForSingleObject(queue->event, INFINITE);
	}
	return H_OK;
}

i8 command_context_fail_handler(directx_queue* queue, command_context_fails fail_code)
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
