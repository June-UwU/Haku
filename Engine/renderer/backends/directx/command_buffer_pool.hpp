#pragma once

#include "defines.hpp"
#include "directx_types.inl"


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
void return_directx_allocator(directx_allocator* obj);
