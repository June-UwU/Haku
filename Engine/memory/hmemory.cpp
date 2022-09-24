#include "hmemory.hpp"
#include "core/logger.hpp"
#include "platform/platform.hpp"

// TODO : write a read write test and check test for the memory subsystem

static u64 total_allocated = 0;
static u64 memory_tracker_array[MEMORY_TAG_COUNT]{};

i8 hmemory_initialize()
{
	i8 ret = platform_memory_initialize();
	return ret;
}

void hmemory_shutdown()
{
	platform_memory_shutdown();
}

void* hmemory_alloc(u64 size, memory_tag tag)
{
	total_allocated += size;
	memory_tracker_array[tag] += size;
	return platform_allocate(size, false);
}

void* hmemory_alloc_zeroed(u64 size, memory_tag tag)
{
	void* ret_ptr =
		hmemory_alloc(size, tag); // since a heap allocation can't assure a zeroed page it is zeroed out manually
	platform_zero_memory(ret_ptr, size);
	return ret_ptr;
}

void hmemory_free(void* block, memory_tag tag)
{
	u64 size = platform_alloc_size(block);
	total_allocated -= size;
	memory_tracker_array[tag] -= size;
	platform_free(block, false);
}

void hmemory_copy(void* dest, void* src, u64 size)
{
	platform_copy_memory(dest, src, size);
}

void hmemory_set(void* dest, i8 val, u64 size)
{
	platform_set_memory(dest, val, size);
}

void hlog_memory_report()
{
	const char* unit = nullptr;
	HLINFO("SubSystem Memory Allocation Report");
	for (u64 i = 0; i < MEMORY_TAG_COUNT; i++)
	{
		u64			size = memory_tracker_array[i];
		if (size < BYTESINKiB(1))
		{
			unit = unit_map[0];
		}
		else if (size < BYTESINMiB(1))
		{
			unit = unit_map[1];
		}
		else if (size < BYTESINGiB(1))
		{
			unit = unit_map[2];
		}
		else
		{
			unit = unit_map[3];
		}

		HLINFO(memory_tag_lookup[i], size, unit);
	}
}

void hmemory_test(void)
{
	void* mem_ptr[MEMORY_TAG_COUNT][page_slice]{};

	HLINFO("\n\n\n\n\nMEMORY TEST INITIAL REPORT\n\n");
	hlog_memory_report();

	for (u32 i = 0; i < MEMORY_TAG_COUNT; i++)
	{
		for (u32 j = 0; j < page_slice; j++)
		{
			mem_ptr[i][j] = hmemory_alloc(single_alloc_cap, static_cast<memory_tag>(i));
		}
	}

	// read write test

	for (u32 i = 0; i < MEMORY_TAG_COUNT; i++)
	{
		for (u32 j = 0; j < page_slice; j++)
		{
			for (u32 k = 0; k < 256; k++)
			{
				char* handle = reinterpret_cast<char*>(mem_ptr[i][j]);
				handle += k;
				*handle = k;
			}
		}
	}

	HLINFO("\n\nMemory Write done");

	for (u32 i = 0; i < MEMORY_TAG_COUNT; i++)
	{
		for (u32 j = 0; j < page_slice; j++)
		{
			for (u32 k = 0; k < 256; k++)
			{
				char* handle = reinterpret_cast<char*>(mem_ptr[i][j]);
				handle += k;
				if (*handle != static_cast<char>(k))
				{
					HLCRIT("Memory Read Write test failure");
				}
			}
		}
	}

	HLINFO("Memory Read Back Done");

	HLINFO("\n\n\n\n\nMEMORY TEST ALLOCATION REPORT\n");
	hlog_memory_report();

	for (u32 i = 0; i < MEMORY_TAG_COUNT; i++)
	{
		for (u32 j = 0; j < page_slice; j++)
		{
			hmemory_free(mem_ptr[i][j], static_cast<memory_tag>(i));
		}
	}

	HLINFO("\n\n\n\n\nMEMORY  TEST DEALLOCATION REPORT\n");
	hlog_memory_report();
}