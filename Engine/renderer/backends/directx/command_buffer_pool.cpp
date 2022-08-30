#include "command_buffer_pool.hpp"

#include "core/logger.hpp"
#include "core/hmemory.hpp"

static u64 pool_capacity;
directx_allocator* pool[HK_COMMAND_MAX];


i8 command_pool_fail_handler();

i8 command_buffer_pool(const directx_context* context,const u64 pool_size)
{
	pool_capacity	= pool_size;
	i8 ret_code	= H_OK;
	HRESULT api_ret_code = S_OK;

	HLINFO("attempting allcation of command buffer pool");
	for(u8 i = 0; i < HK_COMMAND_MAX; i++)
	{
		pool[i] = (directx_allocator*)hmemory_alloc(sizeof(directx_allocator) * pool_size , MEM_TAG_RENDERER);
	}
	
	wchar_t allocator_name[256];

	directx_allocator* init_allocator = pool[HK_COMMAND_RENDER];
	for(u64 j = 0; j < pool_size; j++)
	{	
		init_allocator->state = COMMAND_BUFFER_STATE_NOT_ALLOCATED;
		api_ret_code 			= context->logical_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,__uuidof(ID3D12CommandAllocator), (void**)&init_allocator->allocator);
		if(S_OK != api_ret_code)
		{
			HLEMER("failed to allocate command pools");
			ret_code 		= command_pool_fail_handler();
			return ret_code;
		}
		swprintf(allocator_name, 256u,L"Direct X %s command allocator %lld ",
			COMMAND_NAME[HK_COMMAND_RENDER], j);
		FRIENDLY_NAME(init_allocator->allocator, allocator_name);
		init_allocator			+= 1;			
	}

	init_allocator = pool[HK_COMMAND_COMPUTE];
	for(u64 j = 0; j < pool_size; j++)
	{	
		init_allocator->state = COMMAND_BUFFER_STATE_NOT_ALLOCATED;
		api_ret_code 			= context->logical_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COMPUTE,__uuidof(ID3D12CommandAllocator), (void**)&init_allocator->allocator);
		if(S_OK != api_ret_code)
		{
			HLEMER("failed to allocate command pools");
			ret_code 		= command_pool_fail_handler();
			return ret_code;
		}
		swprintf(allocator_name, 256u,L"Direct X %s command allocator %lld ",
			COMMAND_NAME[HK_COMMAND_COMPUTE], j);
		FRIENDLY_NAME(init_allocator->allocator, allocator_name);
		init_allocator			+= 1;			
	}

	init_allocator = pool[HK_COMMAND_COPY];
	for (u64 j = 0; j < pool_size; j++)
	{
		init_allocator->state = COMMAND_BUFFER_STATE_NOT_ALLOCATED;
		api_ret_code = context->logical_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_COPY,
			__uuidof(ID3D12CommandAllocator), (void**)&init_allocator->allocator);
		if (S_OK != api_ret_code)
		{
			HLEMER("failed to allocate command pools");
			ret_code = command_pool_fail_handler();
			return ret_code;
		}
		swprintf(allocator_name, 256u, L"Direct X %s command allocator %lld ", COMMAND_NAME[HK_COMMAND_COPY], 
			j);
		FRIENDLY_NAME(init_allocator->allocator, allocator_name);
		init_allocator += 1;
	}


	return ret_code;
}

// TODO make the ref count error go away
void command_buffer_pool_shutdown(void)
{
	HLINFO("command buffer pool shutdown");
	for(u64 i = 0; i < HK_COMMAND_MAX; i++)
	{
		directx_allocator* alloc_ptr		= pool[i];
		for(u64 j = 0; j < pool_capacity; j++)
		{
			alloc_ptr->allocator->Release();
		}
	}
}

directx_allocator* request_command_buffer(queue_type type)
{
	directx_allocator* ret_ptr  = nullptr;

	switch(type)
	{
		case  HK_COMMAND_RENDER:
			{
				ret_ptr			= pool[HK_COMMAND_RENDER];
				break;
			}
		case HK_COMMAND_COPY:
			{
				ret_ptr		 	= pool[HK_COMMAND_COPY];
				break;
			}
		case HK_COMMAND_COMPUTE:
			{
				ret_ptr			= pool[HK_COMMAND_COMPUTE];
				break;
			}
		default:
			HLCRIT(" unknown  queue type passed");
			return nullptr;
	}

	for(u64 i = 0; i < pool_capacity; i++)
	{
		if(COMMAND_BUFFER_STATE_NOT_ALLOCATED == ret_ptr->state)
		{
			ret_ptr->state= COMMAND_BUFFER_STATE_READY;
			return ret_ptr;
		}
		ret_ptr					+= 1;
	}

	// TODO : do a full gpu flush and make all allocators available at this point and issue warning >_<
	HLCRIT("no allocator available");
	return nullptr;
}
void return_directx_allocator(directx_allocator* obj)
{
	obj->state					= COMMAND_BUFFER_STATE_NOT_ALLOCATED;
}


i8 command_pool_fail_handler()
{

	for (u64 i = 0; i < HK_COMMAND_MAX; i++)
	{
		directx_allocator* allocator = pool[i];
		for (u64 i = 0; i < pool_capacity; i++)
		{
			if (nullptr != allocator->allocator)
			{
				allocator->allocator->Release();
			}
			allocator += 1;
		}
	}
	return H_FAIL;
}
