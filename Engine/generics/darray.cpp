#include "darray.hpp"
#include "core/logger.hpp"
#include "memory/hmemory.hpp"

// TODO : subtle bug on size(darray ptr function)

// initial count for elements that can be held inside the darray later used as a multiplying constant
constexpr u64 DARRAY_INIT_SIZE =
	8u; // Initial count for elements that can be held inside the darray later used as a multiplying constant
constexpr u64 DARRAY_EXPAND_MARGIN = 4u; // the space the is left in a darray that triggers a rellocation of darray

// @breif 	this is a attribute structure that is kept infront of the darray (i8*) that keeps track of the darray state
// and property
typedef struct darray_attr
{
	u64 size;
	u64 capacity;
	u64 element_size;
	u64 multiplier;
} darray_attr;

// helper function to get a pointer to the header struct
darray_attr* get_darray_attr(darray ptr)
{
	i8*			 byte_ptr = reinterpret_cast<i8*>(ptr);
	darray_attr* attr	  = (darray_attr*)(byte_ptr - sizeof(struct darray_attr));
	return attr;
}

// helper function to expand a darray once space is deemed too low
darray expand_darray(darray ptr) // darray ptr is i8**
{
	darray_attr* attr = get_darray_attr(ptr);

	attr->multiplier = attr->multiplier + 1;
	attr->capacity	 = attr->multiplier * DARRAY_INIT_SIZE;

	darray_attr* new_darray = (darray_attr*)hmemory_alloc(
		((attr->capacity * attr->element_size) + sizeof(struct darray_attr)), MEM_TAG_DARRAY);

	new_darray->multiplier	 = attr->multiplier;
	new_darray->capacity	 = attr->capacity;
	new_darray->element_size = attr->element_size;
	new_darray->size		 = attr->size;

	u64 copy_byte_count = attr->element_size * attr->size;

	i8* swap_array = reinterpret_cast<i8*>(new_darray);

	swap_array = swap_array + sizeof(struct darray_attr);

	hmemory_copy(swap_array, ptr, copy_byte_count);

	destroy_darray(ptr);

	return swap_array;
}

darray create_darray(u64 element_size)
{
	// a total of DARRAY_INIT_SIZE bytes and another 4 u64 for darray data

	darray_attr* attr_ptr =
		(darray_attr*)hmemory_alloc((element_size * DARRAY_INIT_SIZE) + sizeof(struct darray_attr), MEM_TAG_DARRAY);

	attr_ptr->size		   = 0;
	attr_ptr->capacity	   = DARRAY_INIT_SIZE;
	attr_ptr->element_size = element_size;
	attr_ptr->multiplier   = 1;

	i8* ret_ptr = reinterpret_cast<i8*>(attr_ptr);

	ret_ptr = ret_ptr + sizeof(darray_attr);

	return ret_ptr; // return the array
}

void destroy_darray(darray ptr)
{
	i8* byte_ptr = reinterpret_cast<i8*>(ptr); // make the ptr a byte ptr

	byte_ptr = byte_ptr - sizeof(struct darray_attr); // calculate the orginal ptr

	hmemory_free(byte_ptr, MEM_TAG_DARRAY);
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

	if (0 == attr->size)
	{
		return true;
	}

	return false;
}

darray push_back(darray ptr, void* obj)
{
	darray_attr* attr = get_darray_attr(ptr);

	u64 size = attr->size;

	u64 element_size = attr->element_size;

	if (DARRAY_EXPAND_MARGIN > (attr->capacity - attr->size))
	{
		ptr = expand_darray(ptr);
	}

	i8* entry_ptr = reinterpret_cast<i8*>(ptr);

	entry_ptr = (entry_ptr + (size * element_size));

	hmemory_copy(entry_ptr, obj, element_size);

	attr = get_darray_attr(ptr);
	attr->size += 1;

	return ptr;
}

darray pop_back(darray ptr)
{
	darray_attr* attr = get_darray_attr(ptr);

	if (0 == attr->size)
	{
		HLCRIT("darray popping on zero size : undefined behaviour");
		return ptr;
	}

	attr->size -= 1;

	return ptr;
}

void darray_test()
{
	darray ptr = create_darray(sizeof(u64));

	HLINFO("Darray Initialized \n\n\n");

	hlog_memory_report();

	u64 init_cap = capacity(ptr);

	// push back test
	for (u64 i = 0; i < 2 * init_cap; i++)
	{
		ptr = push_back(ptr, &i);
		if (i == init_cap)
		{
			HLINFO("Darray capacity exanded, capacity : %d\n\n\n\n", capacity(ptr));
			hlog_memory_report();
		}
	}

	u64* val_ptr = (u64*)ptr;

	for (u64 i = 0; i < size(ptr); i++)
	{
		HLINFO(" %d", val_ptr[i]);
	}

	// pop_back test
	for (u64 i = 0; i < 6; i++)
	{
		pop_back(ptr);
	}

	for (u64 i = 0; i < size(ptr); i++)
	{
		HLINFO("%d", val_ptr[i]);
	}

	destroy_darray(ptr);

	HLINFO("Darray Dellocation");
	hlog_memory_report();
}

// @breif 	routine to insert element at the position ,the position must be inside capacity or it's a failure with a
// critiical warning for missing data(_DEBUG builds)
// @param 	: darray pointer
// @param	: pointer to object to be copied
// @param	: position
void insert_at(darray ptr, void* obj, u64 pos)
{
	HASSERT(false);
}
// @breif	routine to remove from the position (must be in bounds and warns when a over the size or under the size remove
// is called on _DEBUG builds
// @param	: pointer to the darray
// @param	: position to be removed
darray remove_at(darray ptr, u64 pos)
{
	darray_attr* attr = get_darray_attr(ptr);

	u64 size = attr->size;

	u64 element_size = attr->element_size;

	if (pos > (size - 1))
	{
		HLCRIT("Remove called at out of bounds area : %d", pos);
	}

	i8* byte_ptr = reinterpret_cast<i8*>(ptr);

	byte_ptr = byte_ptr + (pos * element_size);

	i8* copy_ptr = byte_ptr;

	copy_ptr = copy_ptr + element_size;

	for (u64 i = pos; i < size; i++)
	{
		hmemory_copy(byte_ptr, copy_ptr, element_size);
		byte_ptr = byte_ptr + element_size;
		copy_ptr = copy_ptr + element_size;
	}
	attr->size = attr->size - 1;

	return ptr;
}
