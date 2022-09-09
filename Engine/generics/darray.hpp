#pragma once

#include "defines.hpp"

// Haku dynamic array

// size 	: keeps track of the size
// capacity 	: keeps the current capacity of the array
// element size : size in bytes if the structure that the d array is being allocated for
// mulitplier	: size expansion of multipler

// @breif 	macro to create a darray by passing the struct
#define HAKU_CREATE_DARRAY(container_element) create_darray(sizeof(container_element))

typedef void* darray;

typedef enum
{
	DARRAY_SIZE,		 // array current size
	DARRAY_CAPACITY,	 // array current capacity
	DARRAY_ELEMENT_SIZE, // array element size in bytes
	DARRAY_MULTIPLIER	 // internally used multiplier that is used for allocation
} darray_prop;

// @breif	routine to initialize a darray
// @param	: size of the element
// @return	: returns H_OK on sucess else H_FAIL
HAPI darray create_darray(u64 element_size);

// @breif	routine to destroy the darray
// @param	: darray
HAPI void destroy_darray(darray ptr);

// @breif	generic routine to get darray property
// @param	: darray pointer
// @param	: enum for ptr
// @return	: param for enum
HAPI u64 get_darray_property(darray ptr, darray_prop prop);

// @breif 	return capacity
// @param	: in ptr to darray
HAPI u64 capacity(darray ptr);

// @breif	return current size in bytes
// @param	: pointer to the array
// @return 	: return the current size of the darray
HAPI u64 size(darray ptr);

// @breif 	returns true if empty else false
// @param	: in pointer to the darray
// @return	: true on empty else false
HAPI bool empty(darray ptr);

// @breif	routine to add and object back to the array
// @param 	: darray pointer
// @param	: pointer to a object (object is memcpy-ed for now
HAPI darray push_back(darray ptr, void* obj);

// @breif 	routine to pop out of the darray form the back
// @param	: darray pointer
HAPI darray pop_back(darray ptr);

// @breif 	routine to insert element at the position ,the position must be inside capacity or it's a failure with a
// critiical warning for missing data(_DEBUG builds)
// @param 	: darray pointer
// @param	: pointer to object to be copied
// @param	: position
HAPI void insert_at(darray ptr, void* obj, u64 pos);

// @breif	routine to remove from the position (must be in bounds and warns when a over the size or under the size remove
// is called on _DEBUG builds
// @param	: pointer to the darray
// @param	: position to be removed
HAPI darray remove_at(darray ptr, u64 pos);

// @breif 	routine to test darray functionalites
void darray_test();
