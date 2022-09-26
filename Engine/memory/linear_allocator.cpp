/*****************************************************************//**
 * \file   linear_allocator.cpp
 * \brief  haku linear allocator implementations
 * 
 * \author June
 * \date   September 2022
 *********************************************************************/
#include "hmemory.hpp"	
#include "linear_allocator.hpp"
#include "core/logger.hpp"

linear_allocator* create_linear_allocator(u64 size)
{
	linear_allocator* ret_ptr = (linear_allocator*)hmemory_alloc(sizeof(linear_allocator), MEM_TAG_ALLOCATOR);
	ret_ptr->offset			  = 0;
	ret_ptr->size_in_bytes	  = size;
	ret_ptr->memory			  = hmemory_alloc(size, MEM_TAG_ALLOCATOR);

	return ret_ptr;
}

void destroy_linear_allocator(linear_allocator* alloc_ptr)
{
	hmemory_free(alloc_ptr->memory, MEM_TAG_ALLOCATOR);
	hmemory_free(alloc_ptr, MEM_TAG_ALLOCATOR);
}

void* linear_allocate(linear_allocator* allocator, u64 size)
{
	void* ret_ptr = nullptr;
	// don't try to cheese the system >w<
	if (0 == size)
	{
		HLCRIT("memory allocation failure \n \t request size : %d bytes \n \t total size : %d bytes\n \t free size : %d ", size,
			allocator->size_in_bytes, allocator->size_in_bytes - allocator->offset);
		return ret_ptr;
	}

	if ((allocator->offset + size) >= allocator->size_in_bytes)
	{
		return ret_ptr;
	}
	ret_ptr = (i8*)allocator->memory + allocator->offset;
	allocator->offset += size;
	return ret_ptr;
}

i8 linear_allocator_reset(linear_allocator* allocator)
{
	allocator->offset = 0;

	return H_OK;
}
