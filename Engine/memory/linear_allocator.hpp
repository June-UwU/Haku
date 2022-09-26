/*****************************************************************//**
 * \file   linear_allocator.hpp
 * \brief  haku linear allocator
 * 
 * \author June
 * \date   September 2022
 *********************************************************************/
#pragma once

#include "defines.hpp"

/** linear allocator for haku */
typedef struct linear_allocator
{
	/** current offset */
	u64 offset;

	/** total size of the memory */
	u64 size_in_bytes;

	/** void pointer to memory */
	void* memory;
} linear_allocator;
/**  
* routine to create allocator
* \param	: size of the backend memory a pool
* \return	: pointer to the allocator
*/
HAPI linear_allocator* create_linear_allocator(u64 size);

/**
 * routine to destroy a allocator.
 * 
 * \param alloc_ptr pointer  to the allocator
 */
HAPI void destroy_linear_allocator(linear_allocator* alloc_ptr);

/**
 * routine to allocate a block of memory form the allocator.
 * 
 * \param allocator pointer to allocator to allocate from
 * \param size size in bytes to allocate form
 * \return pointer to the block for allocated memory
 */
HAPI void* linear_allocate(linear_allocator* allocator, u64 size);

// @breif	
/**
 * routine to reset all allocation.
 * 
 * \param allocator pointer to the linear allocator to reset
 * \return H_OK on sucess and H_FAIL on failure
 */
HAPI i8 linear_allocator_reset(linear_allocator* allocator);




