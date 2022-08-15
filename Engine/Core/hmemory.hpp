#pragma once


#include "defines.hpp"


// Haku memory subsystem
// currently there exits a platform limitation as a memory alloc call has a cap on size 
// x86 		: 512  KB
// x64		: 1024 KB


typedef enum memory_tag
{
	MEM_TAG_LOGGER,		// logger subsystem allocations
	MEM_TAG_MEMORY,		// memory subsystem allocations
	MEM_TAG_PLATFORM,	// platform subsystem allocations
	MEM_TAG_GAME,		// game allocations

	MEMORY_TAG_COUNT 	// last enum for size purposes
}memory_tag;

// @breif	memory subsystem initialize
i8   hmemory_initialize();

// @brief	memory subsystem shutdown
void hmemory_shutdown();

// @breif 	memory allocation routine
// @param	: size of allocation
// @param	: memory tag
// @return 	: void pointer to memory
void* hmemory_alloc(u64 size, memory_tag tag);

// @breif	memory allocation that returns zeroed memory
// @param	: size of allocation
// @param	: memory tag
// @return 	: void pointer to memory
void* hmemory_alloc_zeroed(u64 size, memory_tag);

// TODO : make the free independant of the memory tag

// @breif 	memory free routine
// @param	: pointer to the block to free
void hmemory_free(void* block, memory_tag tag);

// @breif	debug routine to log memory report
void hlog_memory_report();

// @brief 	memory test routine
void hmemory_test(void);
