#include "linear_allocator.hpp"
#include "hmemory.hpp"	�


linear_allocator* create_linear_allocator(u64 size )
{
	linear_allocator* ret_ptr 	= (linear_allocator*)hmemory_alloc(sizeof(linear_allocator),MEM_TAG_ALLOCATOR);
	ret_ptr->offset 		= 0;
	ret_ptr->size_in_bytes		= size;
	ret_ptr->memory 		= hmemory_alloc(size,MEM_TAG_ALLOCATOR);
	
	return ret_ptr;
}

void destroy_linear_allocator(linear_allocator* alloc_ptr)
{
	hmemory_free(alloc_ptr->memory,MEM_TAG_ALLOCATOR);
	hmemory_free(alloc_ptr, MEM_TAG_ALLOCATOR);
}
void* linear_allocate(linear_allocator* allocator, u64 size)
{
	void* ret_ptr 			= nullptr;
	if((allocator->allocated + size) >= allocator->size_in_bytes)
	{
		return ret_ptr;
	}
	ret_ptr				= (i8*)allocator->memory + allocator->offset;
	allocator->offset		+= size;
	return ret_ptr;
}

