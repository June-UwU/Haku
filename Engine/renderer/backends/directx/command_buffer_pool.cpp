#include "command_buffer_pool.hpp"

#include "core/logger.hpp"
#include "memory/hmemory.hpp"

static u64				  POOL_CAPACITY;
static directx_cc* pool;

i8 command_pool_fail_handler();

// TODO : properly handle failures
i8 command_buffer_pool(const directx_device* device, const u64 pool_size)
{
	// HLWARN("Pool is being limited to 1 make it to %lld",pool_size);
	POOL_CAPACITY		 = pool_size;
	i8		ret_code	 = H_OK;
	HRESULT api_ret_code = S_OK;

	HLINFO("attempting allcation of command buffer pool");
	pool = (directx_cc*)hmemory_alloc(sizeof(directx_cc) * POOL_CAPACITY , MEM_TAG_RENDERER);

	wchar_t allocator_name[256];

	directx_cc* init_cc = pool;
	for (u64 j = 0; j < POOL_CAPACITY; j++)
	{
		swprintf(allocator_name, 256u, L"Direct X %s command allocator %lld ", COMMAND_NAME[HK_COMMAND_RENDER], j);
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
	}

	return ret_code;
}

// TODO make the ref count error go away
void command_buffer_pool_shutdown(void)
{
	HLINFO("command buffer pool shutdown");
	directx_cc* cc = pool;
	for (u64 j = 0; j < POOL_CAPACITY; j++)
	{
		cc->allocator->Release();
		cc->commandlist->Release();
		cc += 1;
	}
	hmemory_free(pool, MEM_TAG_RENDERER);
}

directx_cc* request_dxcc()
{
	directx_cc* ret_ptr			 = pool;
	u64				   highest_fence_val = 0;


	for (u64 i = 0; i < POOL_CAPACITY; i++)
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
				HLEMER("commandlist reset failure ",);
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

void reintroduce_dxcc(u64 fence_value)
{
	directx_cc* alloc_ptr = pool;
	for (u64 j = 0; j < POOL_CAPACITY; j++)
	{
		if (alloc_ptr->fence_val < fence_value)
		{
			alloc_ptr->state = COMMAND_BUFFER_STATE_NOT_ALLOCATED;
		}
		alloc_ptr += 1;
		// else check if the next command list is completed
	}
}

i8 command_pool_fail_handler()
{
	directx_cc* cc = pool;
	for (u64 i = 0; i < POOL_CAPACITY; i++)
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
	return H_FAIL;
}
