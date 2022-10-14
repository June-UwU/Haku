/*****************************************************************/ /**
* \file   hmemory.hpp
* \brief  Haku memory subsystem
*
* \author June
* \date   September 2022
*********************************************************************/
#pragma once

#include "defines.hpp"

// Haku memory subsystem
// currently there exits a platform limitation as a memory alloc call has a cap on size
// x86 		: 512  KB
// x64		: 1024 KB

/** testing purpose page slice */
constexpr u32 page_slice = 10;

/** current memory allocation capacity assuming the worst as x86 systems */
constexpr u32 single_alloc_cap = BYTESINKiB(512);

/** memory tag to let the memory subsystem keep track on who made the allocations */
typedef enum memory_tag
{
	/** unknown allocations */
	MEM_TAG_UNKNOWN,

	/** logger subsystem allocations */
	MEM_TAG_LOGGER,

	/** memory subsystem allocations */
	MEM_TAG_MEMORY,

	/** platform subsystem allocations */
	MEM_TAG_PLATFORM,

	/** renderer subsystem allocations */
	MEM_TAG_RENDERER,

	/** game allocations */
	MEM_TAG_GAME,

	/** generic darray */
	MEM_TAG_DARRAY,

	/** generic c_queue */
	MEM_TAG_CQUEUE,

	/** allcators allocation */
	MEM_TAG_ALLOCATOR,

	/** allocation done by core routines */
	MEM_TAG_CORE,

	/** memory allcation by generic singly linked list */
	MEM_TAG_SLIST,

	/** memory allocations of hash table */
	MEM_TAG_HASH_TABLE,

	/** memory allocations made for queue */
	MEM_TAG_QUEUE,

	/** memory allocations made by the hash map */
	MEM_TAG_HASH_MAP,

	/** memory allocation made by the skip list */
	MEM_TAG_SKIP_LIST,

	/** last enum for size purposes */
	MEMORY_TAG_COUNT
} memory_tag;

constexpr const char* unit_map[]{ "Bytes", "KiB", "MiB", "GiB" };

constexpr const char* memory_tag_lookup[MEMORY_TAG_COUNT]{
	"\t Unknown 		: %d %s", "\t Logger 		: %d %s", "\t Memory 		: %d %s", "\t Platform	: %d %s",
	"\t Renderer	: %d %s",		  "\t Game		: %d %s",	   "\t Darray		: %d %s", "\t CQueue		: %d %s",
	"\t Allocators  : %d %s",	  "\t Core        : %d %s",	   "\t Slist		: %d %s",   "\t Hash Table		: %d %s",
	"\t Queue  : %d %s" ,"Hash map  : %d %s","Skip list  : %d %s"
};

/**
 * memory subsystem initialize.
 *
 * \return H_OK on sucess
 */
i8 hmemory_initialize();

/**
 * memory subsystem shutdown.
 */
void hmemory_shutdown();

/**
 * memory allocation routine.
 *
 * \param size size of allocation
 * \param tag memory tag
 * \return void pointer to memory
 */
HAPI void* hmemory_alloc(u64 size, memory_tag tag);

/**
 * memory allocation that returns zeroed memory.
 *
 * \param size size of allocation
 * \param tag memory tag
 * \return void pointer to memory
 */
HAPI void* hmemory_alloc_zeroed(u64 size, memory_tag tag);

// TODO : make the free independant of the memory tag

/**
 * memory free routine.
 *
 * \param block pointer to the block to free
 * \param tag memory tag
 */
HAPI void hmemory_free(void* block, memory_tag tag);

/**
 * memcpy routine.
 *
 * \param dest destination for the memcpy
 * \param src source to get copy from
 * \param size size of the copy in bytes
 */
HAPI void hmemory_copy(void* dest, void* src, u64 size);

/**
 * memset routine.
 *
 * \param dest destination to be set
 * \param val byte value to be set
 * \param size size of the set in bytes
 */
HAPI void hmemory_set(void* dest, i8 val, u64 size);

/**
 * debug routine to log memory report.
 */
HAPI void hlog_memory_report();

/**
 * memory test routine.
 */
void hmemory_test(void);
