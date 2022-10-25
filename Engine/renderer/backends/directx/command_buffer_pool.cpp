#include "command_buffer_pool.hpp"

#include "core/logger.hpp"
#include "memory/hmemory.hpp"

static u64				  pool_capacity;
static directx_cc* pool[HK_COMMAND_MAX];

i8 command_pool_fail_handler();

// TODO : properly handle failures
i8 command_buffer_pool(const directx_device* device, const u64 pool_size)
{
	// HLWARN("Pool is being limited to 1 make it to %lld",pool_size);
	pool_capacity		 = pool_size;
	i8		ret_code	 = H_OK;
	HRESULT api_ret_code = S_OK;

	HLINFO("attempting allcation of command buffer pool");
	for (u8 i = 0; i < HK_COMMAND_MAX; i++)
	{
		pool[i] = (directx_cc*)hmemory_alloc(sizeof(directx_cc) * pool_size, MEM_TAG_RENDERER);
	}

	wchar_t allocator_name[256];

	directx_cc* init_cc = pool[HK_COMMAND_RENDER];
	for (u64 j = 0; j < pool_size; j++)
	{
		init_cc->type = HK_COMMAND_RENDER;
		init_cc->fence_val = 0;
		init_cc->state	  = COMMAND_BUFFER_STATE_NOT_ALLOCATED;
		api_ret_code			  = device->logical_device->CreateCommandAllocator(
			 D3D12_COMMAND_LIST_TYPE_DIRECT, __uuidof(ID3D12CommandAllocator), (void**)&init_cc->allocator);
		if (S_OK != api_ret_code)
		{
			HLEMER("failed to allocate command allocator");
			ret_code = command_pool_fail_handler();
			return ret_code;
		}
		FRIENDLY_NAME(init_cc->allocator, allocator_name);
		
		// render command list create
		api_ret_code = device->logical_device->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_DIRECT,
			init_cc->allocator,
			nullptr,
			__uuidof(ID3D12GraphicsCommandList),
			(void**)&init_cc->commandlist);
		FRIENDLY_NAME(init_cc->commandlist, L"render commandlist");
		
		if (S_OK != api_ret_code)
		{
			HLEMER("failed to allocate command list");
			ret_code = command_pool_fail_handler();
			return H_FAIL;
		}
		api_ret_code = init_cc->commandlist->Close();
		if (S_OK != api_ret_code)
		{
			HLEMER("failed to close command list");
			ret_code = command_pool_fail_handler();
			return H_FAIL;
		}
		init_cc += 1;
		swprintf(allocator_name, 256u, L"Direct X %s command allocator %lld ", COMMAND_NAME[HK_COMMAND_RENDER], j);
	}

	init_cc = pool[HK_COMMAND_COMPUTE];
	for (u64 j = 0; j < pool_size; j++)
	{
		init_cc->type	  = HK_COMMAND_COMPUTE;
		init_cc->fence_val = 0;
		init_cc->state	  = COMMAND_BUFFER_STATE_NOT_ALLOCATED;
		api_ret_code			  = device->logical_device->CreateCommandAllocator(
			 D3D12_COMMAND_LIST_TYPE_COMPUTE, __uuidof(ID3D12CommandAllocator), (void**)&init_cc->allocator);
		if (S_OK != api_ret_code)
		{
			HLEMER("failed to allocate command pools");
			ret_code = command_pool_fail_handler();
			return ret_code;
		}
		FRIENDLY_NAME(init_cc->allocator, allocator_name);
		// compute command list create
		api_ret_code = device->logical_device->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_COMPUTE,
			init_cc->allocator,
			nullptr,
			__uuidof(ID3D12GraphicsCommandList),
			(void**)&init_cc->commandlist);
		FRIENDLY_NAME(init_cc->commandlist, L"compute commandlist");

		if (S_OK != api_ret_code)
		{
			HLEMER("failed to allocate command list");
			ret_code = command_pool_fail_handler();
			return H_FAIL;
		}

		api_ret_code = init_cc->commandlist->Close();
		if (S_OK != api_ret_code)
		{
			HLEMER("failed to close command list");
			ret_code = command_pool_fail_handler();
			return H_FAIL;
		}

		
		swprintf(allocator_name, 256u, L"Direct X %s command allocator %lld ", COMMAND_NAME[HK_COMMAND_COMPUTE], j);		
		init_cc += 1;
	}

	init_cc = pool[HK_COMMAND_COPY];
	for (u64 j = 0; j < pool_size; j++)
	{
		init_cc->type = HK_COMMAND_COPY;
		init_cc->fence_val = 0;
		init_cc->state	  = COMMAND_BUFFER_STATE_NOT_ALLOCATED;
		api_ret_code			  = device->logical_device->CreateCommandAllocator(
			 D3D12_COMMAND_LIST_TYPE_COPY, __uuidof(ID3D12CommandAllocator), (void**)&init_cc->allocator);
		if (S_OK != api_ret_code)
		{
			HLEMER("failed to allocate command pools");
			ret_code = command_pool_fail_handler();
			return ret_code;
		}
		FRIENDLY_NAME(init_cc->allocator, allocator_name);

		api_ret_code = device->logical_device->CreateCommandList(
			0,
			D3D12_COMMAND_LIST_TYPE_COPY,
			init_cc->allocator,
			nullptr,
			__uuidof(ID3D12GraphicsCommandList),
			(void**)&init_cc->commandlist);
		FRIENDLY_NAME(init_cc->commandlist, L"copy commandlist");

		if (S_OK != api_ret_code)
		{
			HLEMER("failed to allocate command list");
			ret_code = command_pool_fail_handler();
			return H_FAIL;
		}
		api_ret_code = init_cc->commandlist->Close();
		if (S_OK != api_ret_code)
		{
			HLEMER("failed to close command list");
			ret_code = command_pool_fail_handler();
			return H_FAIL;
		}

		swprintf(allocator_name, 256u, L"Direct X %s command allocator %lld ", COMMAND_NAME[HK_COMMAND_COPY], j);
		
		init_cc += 1;
	}

	return ret_code;
}

// TODO make the ref count error go away
void command_buffer_pool_shutdown(void)
{
	HLINFO("command buffer pool shutdown");
	for (u64 i = 0; i < HK_COMMAND_MAX; i++)
	{
		directx_cc* cc = pool[i];
		for (u64 j = 0; j < pool_capacity; j++)
		{
			cc->allocator->Release();
			cc->commandlist->Release();
			cc += 1;
		}
		hmemory_free(pool[i], MEM_TAG_RENDERER);
	}
}

directx_cc* request_dxcc(queue_type type)
{
	directx_cc* ret_ptr			 = nullptr;
	u64				   highest_fence_val = 0;

	switch (type)
	{
	case HK_COMMAND_RENDER:
	{
		ret_ptr = pool[HK_COMMAND_RENDER];
		break;
	}
	case HK_COMMAND_COPY:
	{
		ret_ptr = pool[HK_COMMAND_COPY];
		break;
	}
	case HK_COMMAND_COMPUTE:
	{
		ret_ptr = pool[HK_COMMAND_COMPUTE];
		break;
	}
	default:
		HLCRIT(" unknown  queue type passed");
		return nullptr;
	}

	for (u64 i = 0; i < pool_capacity; i++)
	{
		if (ret_ptr->fence_val > highest_fence_val)
		{
			highest_fence_val = ret_ptr->fence_val;
		}
		if (COMMAND_BUFFER_STATE_NOT_ALLOCATED == ret_ptr->state)
		{
			ret_ptr->state = COMMAND_BUFFER_STATE_RECORDING;
			HRESULT win32_rv = ret_ptr->commandlist->Reset(ret_ptr->allocator, NULL);
			if (S_OK != win32_rv)
			{
				HLEMER("commandlist reset failure : \n \t %ls",COMMAND_NAME[type]);
				return nullptr;
			}
			return ret_ptr;
		}
		ret_ptr += 1;
	}

	// TODO : do a full gpu flush and make all allocators available at this point and issue warning >_<
	HLCRIT("no allocator available, highest fence value : %lld", highest_fence_val);
	return nullptr;
}
void return_directx_cc(directx_cc* obj)
{
	obj->state = COMMAND_BUFFER_STATE_NOT_ALLOCATED;
}

void reintroduce_dxcc(u64 fence_value)
{
	for (u64 i = 0; i < HK_COMMAND_MAX; i++)
	{
		directx_cc* alloc_ptr = pool[i];
		for (u64 j = 0; j <= pool_capacity; j++)
		{
			if (alloc_ptr->fence_val < fence_value)
			{
				alloc_ptr->state = COMMAND_BUFFER_STATE_NOT_ALLOCATED;
			}
			// else check if the next command list is completed
			alloc_ptr += 1;
		}
	}
}

i8 command_pool_fail_handler()
{
	for (u64 i = 0; i < HK_COMMAND_MAX; i++)
	{
		directx_cc* cc = pool[i];
		for (u64 i = 0; i < pool_capacity; i++)
		{
			if (nullptr != cc->allocator)
			{
				cc->allocator->Release();
			}
			if (nullptr != cc->commandlist)
			{
				cc->commandlist->Release();
			}
			cc += 1;
		}
	}
	return H_FAIL;
}
