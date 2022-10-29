#pragma once

#include "defines.hpp"
#include "directx_types.INL"

// TODO : implement a wait function to do a gpu flush


/**
 * routine to create a command pool of command allocator object.
 * 
 * \param device directx_device pointer used to create the command_buffer_pool
 * \param pool_size the pool size
 * \return  H_OK on sucess
 */
i8 command_buffer_pool(const directx_device* device, const u64 pool_size);

/**
 * routine to shutdown command buffer pools.
 */
void command_buffer_pool_shutdown(void);

/**
 * request command buffer.
 * 
 * \param type command pool type that points the type 
 * \return directx command allocator
 */
directx_cc* request_dxcc();

/**
 * routine to make the command allocator object back for request.
 * 
 * \param obj command allocator object to be reintroduced to the open pool
 */
void return_directx_cc(directx_cc* obj);

/**
 * routine that reintroduce the allocators thar are done executing in the command queue.
 * 
 * \param fence_value completed fence value
 */
void reintroduce_dxcc(u64 fence_value);


