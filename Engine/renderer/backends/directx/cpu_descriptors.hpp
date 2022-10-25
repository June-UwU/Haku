/*****************************************************************//**
 * \file   cpu_descriptors.hpp
 * \brief  haku directx backend descriptor heap handler and allocator
 * 
 * \author June
 * \date   October 2022
 *********************************************************************/
#pragma once

#include "defines.hpp"
#include <d3d12.h>
/** size of a page */
constexpr const u64 INITIAL_DESCRIPTOR_PAGE_SIZE = BYTESINMiB(4u);

/** size macro denoting a failed allocation */
constexpr const u64 FAILED_ALLOC = -1;

/** structure denoting a descriptor allocation */
typedef struct cdes
{
	/** number of descriptors */
	i64 size;
	
	/** page id from with page allocation was made */
	SIZE_T page_id;
	
	/** allocated handle */
	D3D12_CPU_DESCRIPTOR_HANDLE  ptr;
}cdes;

/**
 * routine to initialize the cpu descriptor allocator.
 * 
 * \returns H_OK on sucess
 */
i8 cpu_descriptor_allocator_initialize(void);

/**
 * routine to allocate cpu descriptor.
 * 
 * \param size of the descriptor allocations
 * \return an allocation (check with is_alloc_fail(cdes*) )
 */
cdes cpu_desc_allocate(u32 size);

/**
 * routine to free an allocation that was made.
 * 
 * \param handle pointer to the previous allocation
 */
void cpu_desc_free(cdes* handle);

/**
 * routine to shutdown the cpu allcator.
 */
void cpu_descriptor_allocator_shutdown(void);

/**
 * check if allocation failed.
 * 
 * \param desc an allocation that was made
 * \return true if allocation was failed
 */
bool is_alloc_fail(cdes* desc);

/**
 * routine to test the allocator.
 */
void test_descriptor(void);




