#include "darray.hpp"
#include "core/hmemory.hpp"
#include "core/logger.hpp"

// initial count for elements that can be held inside the darray later used as a multiplying constant
constexpr 	u64	 DARRAY_INIT_SIZE 	= 64u; // Initial count for elements that can be held inside the darray later used as a multiplying constant
constexpr 	u64 	 DARRAY_EXPAND_MARGIN 	= 8u;  // the space the is left in a darray that triggers a rellocation of darray

// @breif 	this is a attribute structure that is kept infront of the darray (i8*) that keeps track of the darray state and property
typedef struct darray_attr
{
	u64 size;
	u64 capacity;
	u64 element_size;
	u64 multiplier;
}darray_attr;

// helper function to get a pointer to the header struct
darray_attr* get_darray_attr(darray ptr)
{
	return (darray_attr*)(ptr - sizeof(struct darray_attr));
}

// helper function to expand a darray once space is deemed too low
void expand_darray(darray ptr)
{
	darray_attr* attr 	= get_darray_attr(ptr);

	attr->multiplier	+= 1;
	attr->capacity		= attr->multiplier * DARRAY_INIT_SIZE;

	darray_attr* new_darray = (darray_attr*)hmemory_alloc((attr->capacity * attr->element_size),MEM_TAG_DARRAY);

	new_darray->multiplier 	= attr->multiplier;
	new_darray->capacity	= attr->capacity;
	new_darray->element_size= attr->element_size;
	new_darray->size	= attr->size;

	u64 copy_byte_count	= attr->element_size * attr->size;

	new_darray		= new_darray + sizeof(struct darray_attr);
	
	hmemory_copy(new_darray, ptr,copy_byte_count);

	destroy_darray(ptr);

	ptr 			= reinterpret_cast<darray>(new_darray);
}

darray create_darray(u64 element_size)
{
	// a total of DARRAY_INIT_SIZE bytes and another 4 u64 for darray data

	darray_attr* attr_ptr = (darray_attr*)hmemory_alloc((element_size * DARRAY_INIT_SIZE) + sizeof(struct darray_attr), MEM_TAG_DARRAY);
	 

	attr_ptr->size		= 0;
	attr_ptr->capacity	= DARRAY_INIT_SIZE;
	attr_ptr->element_size	= element_size;
	attr_ptr->multiplier	= 1;

	return (darray)(attr_ptr + sizeof(struct darray_attr));  // return the array
}


void destroy_darray(darray ptr)
{
	void* free_ptr =  (void*)(ptr - sizeof(struct darray_attr));
	hmemory_free(free_ptr, MEM_TAG_DARRAY);
}

u64 capacity(darray ptr)
{
	darray_attr* attr = get_darray_attr(ptr);

	return attr->capacity;
}

u64 size(darray ptr)
{
	darray_attr* attr = get_darray_attr(ptr);

	return attr->size;
}

bool empty(darray ptr)
{
	darray_attr* attr = get_darray_attr(ptr);
	
	if( 0 == attr->size)
	{
		return true;
	}

	return false;
}

void push_back(darray ptr,void* obj)
{
	darray_attr* attr 	= get_darray_attr( ptr);

	u64 size 		= attr->size;
	
	u64 element_size	= attr->element_size;

	if( DARRAY_EXPAND_MARGIN > (attr->capacity - attr->size))
	{
		expand_darray(ptr);
	}

	darray entry_ptr 	= (ptr + ( size * element_size));

	hmemory_copy(entry_ptr,obj,element_size);
	
	attr			= get_darray_attr(ptr);
	attr->size		+= 1;
}

void pop_back(darray ptr)
{
	darray_attr* attr	= get_darray_attr(ptr);
	
	if( 0 == attr->size)
	{
		HLCRIT("darray popping on zero size : undefined behaviour");
		return;
	}

	attr->size 		-=1;
}

// @breif 	routine to insert element at the position ,the position must be inside capacity or it's a failure with a critiical warning for missing data(_DEBUG builds)
// @param 	: darray pointer
// @param	: pointer to object to be copied
// @param	: position
void insert_at(darray ptr,void* obj,u64 pos);

// @breif	routine to remove from the position (must be in bounds and warns when a over the size or under the size remove is called on _DEBUG builds
// @param	: pointer to the darray
// @param	: position to be removed
void remove_at(darray ptr,u64 pos);

