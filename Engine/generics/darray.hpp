/*****************************************************************//**
 * \file   darray.hpp
 * \brief  Haku dynamic array
 * 
 * \author June
 * \date   September 2022
 *********************************************************************/
#pragma once

#include "defines.hpp"

/** breif 	macro to create a darray by passing the struct */
#define HAKU_CREATE_DARRAY(container_element) create_darray(sizeof(container_element))

/** type define to mark a dynamic array */
typedef void* darray;

/** enum that is used to mark darray properties */
typedef enum darray_prop
{
	/**  array current size */
	DARRAY_SIZE,		 

	/**  array current capacity */
	DARRAY_CAPACITY,	 

	/** array element size in bytes */
	DARRAY_ELEMENT_SIZE, 

	/**  internally used multiplier that is used for allocation */
	DARRAY_MULTIPLIER	
} darray_prop;

/**
 * routine to initialize a darray.
 * 
 * \param element_size size of the element
 * \return returns H_OK on sucess else H_FAIL
 */
HAPI darray create_darray(u64 element_size);

/**
 * routine to destroy the darray.
 * 
 * \param ptr pointer to darray
 */
HAPI void destroy_darray(darray ptr);

/**
 * generic routine to get darray property.
 * 
 * \param	ptr darray pointer
 * \param prop enum for ptr
 * \return param for enum
 */
HAPI u64 get_darray_property(darray ptr, darray_prop prop);

/**
 * routine to return capacity.
 * 
 * \param ptr  pointer to darray
 * \return the capacity of darray
 */
HAPI u64 capacity(darray ptr);

/**
 * return current size in bytes.
 * 
 * \param ptr pointer to the array
 * \return current size of the darray
 */
HAPI u64 size(darray ptr);

/**
 * returns true if empty else false.
 * 
 * \param ptr pointer to the darray
 * \return true on empty else false
 */
HAPI bool empty(darray ptr);

/**
 * routine to add and object back to the array.
 * 
 * \param ptr darray pointer
 * \param obj pointer to a object (object is memcpy-ed for now)
 * \return the darray with element pushed
 */
HAPI darray push_back(darray ptr, void* obj);

/**
 * routine to pop out of the darray form the back.
 * 
 * \param ptr darray pointer
 * \return darray with the element popped
 */
HAPI darray pop_back(darray ptr);

/**
 * routine to get the number of elements that are in the darray.
 * 
 * \param ptr pointer to dynamic array
 * \return number of elements in the array
 */
HAPI u64 count(darray ptr);

// TODO 
// @breif 	routine to insert element at the position ,the position must be inside capacity or it's a failure with a
// critiical warning for missing data(_DEBUG builds)
// @param 	: darray pointer
// @param	: pointer to object to be copied
// @param	: position
HAPI void insert_at(darray ptr, void* obj, u64 pos);

// TODO
// @breif	routine to remove from the position (must be in bounds and warns when a over the size or under the size remove
// is called on _DEBUG builds
// @param	: pointer to the darray
// @param	: position to be removed
HAPI darray remove_at(darray ptr, u64 pos);

/** routine to test darray functionalites */ 
void darray_test();





