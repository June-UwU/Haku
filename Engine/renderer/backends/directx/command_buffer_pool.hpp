#pragma once

#include "defines.hpp"
#include "directx_types.inl"

// TODO : implement a wait function to do a gpu flush


// @breif 	routine to create a command pool of command allocator object
// @param	: the pool size
// @return 	: H_OK on sucess
i8 command_buffer_pool(const directx_context* context, const u64 pool_size);

// @breif	routine to shutdown command buffer pools
void command_buffer_pool_shutdown(void);

// @breif 	request command buffer
// @param	: command pool type 
// @return	: directx command allocator
directx_allocator* request_command_buffer(queue_type type); 

// @breif	routine to make the command allocator object back for request
// @param	: command allocator object to be reintroduced to the open pool
void return_directx_allocator(directx_allocator* obj);

// @breif 	routine that reintroduce the allocators thar are done executing in the command queue
// @param	: completed fence value
void reintroduce_allocator(u64 fence_value);
