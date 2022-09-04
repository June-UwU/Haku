#pragma once


#include "defines.hpp"


// Haku memory subsystem
// currently there exits a platform limitation as a memory alloc call has a cap on size 
// x86 		: 512  KB
// x64		: 1024 KB


constexpr u32 page_slice = 10;	// forcing atleast two pages of memory
constexpr u32 single_alloc_cap = 512; 	// using x86 mem cap 

typedef enum memory_tag
{
	MEM_TAG_LOGGER,		// logger subsystem allocations
	MEM_TAG_MEMORY,		// memory subsystem allocations
	MEM_TAG_PLATFORM,	// platform subsystem allocations
	MEM_TAG_RENDERER,	// renderer subsystem allocations
	MEM_TAG_GAME,		// game allocations
	MEM_TAG_DARRAY,		// generic darray
	MEM_TAG_CQUEUE,		// generic c_queue
	MEM_TAG_ALLOCATOR,  // allcators allocation

	MEMORY_TAG_COUNT 	// last enum for size purposes
}memory_tag;


constexpr const char* unit_map[]
{
	"Bytes",
	"KiB",
	"MiB",
	"GiB"
};

constexpr const char* memory_tag_lookup[MEMORY_TAG_COUNT]
{
	"\t Logger 		: %d %s",
	"\t Memory 		: %d %s",
	"\t Platform	: %d %s",
	"\t Renderer	: %d %s",
	"\t Game		: %d %s",
	"\t Darray		: %d %s",
	"\t CQueue		: %d %s",
	"\t Allocators  : %d %s"
};

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

// @breif	memcpy routine
// @param	: destination for the memcpy
// @param	: source to get copy from
// @param	: size of the copy in bytes
void hmemory_copy(void* dest, void* src, u64 size);

// @breif	memset  routine
// @param	: destination to be set
// @param 	: byte value to be set
// @param	: size of the set in bytes
void hmemory_set(void* dest,i8 val,u64 size);
// @breif	debug routine to log memory report
void hlog_memory_report();

// @brief 	memory test routine
void hmemory_test(void);
