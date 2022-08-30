#include "command_buffer_pool.hpp"

#include "core/logger.hpp"
#include "core/hmemory.hpp"

static u64 pool_capacity;
directx_allocator* pool[HK_COMMAND_MAX];

i8 command_pool_fail_handler(u64 i , u64 pool_size);

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
	for(u64 i = 0; i < HK_COMMAND_MAX; i++)
	{	
		directx_allocator* init_allocator	= pool[i];
		for(u64 j = 0; j < pool_size; j++)
		{	
			init_allocator->state = COMMAND_BUFFER_STATE_NOT_ALLOCATED;
			api_ret_code 			= context->logical_device->CreateCommandAllocator(D3D12_COMMAND_LIST_TYPE_DIRECT,__uuidof(ID3D12CommandAllocator), (void**)&init_allocator->allocator);
			if(S_OK != api_ret_code)
			{
				HLEMER("failed to allocate command pools");
				ret_code 		= command_pool_fail_handler(i,j);
				return ret_code;
			}
			swprintf(allocator_name, 256u,L"Direct X %s command allocator %lld location : %p",COMMAND_NAME[i], j,init_allocator);
			//HLWARN("%ls", allocator_name);
			FRIENDLY_NAME(init_allocator->allocator, allocator_name);
			init_allocator			+= 1;			
		}
	}

	return ret_code;
}

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

// @breif 	request command buffer
// @param	: command pool type 
// @return	: directx command allocator
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
// @breif	routine to make the command allocator object back for request
void return_directx_allocator(directx_allocator* obj)
{
	obj->state					= COMMAND_BUFFER_STATE_NOT_ALLOCATED;
}


i8 command_pool_fail_handler(u64 ci , u64 pool_size)
{
	for(u64 i = 0; i < ci; i++)
	{
		directx_allocator* clean	= pool[i];
		for(u64 j = 0; j < pool_size; j++)
		{
			clean->allocator->Release();
		}
	}

	return H_FAIL;
}
