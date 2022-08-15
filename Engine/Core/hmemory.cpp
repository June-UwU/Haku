#include "logger.hpp"
#include "hmemory.hpp"
#include "platform\platform.hpp"

static u64 total_allocated = 0;
static u64 memory_tracker_array[MEMORY_TAG_COUNT]{};

constexpr u32 page_slice 		= 10;	// forcing atleast two pages of memory
constexpr u32 single_alloc_cap 	= 512; 	// using x86 mem cap 

static const char* memory_tag_lookup[MEMORY_TAG_COUNT]
{
	"\t Logger 	: %d %s",
	"\t Memory 	: %d %s",
	"\t Platform	: %d %s",
	"\t Game	: %d %s"
};

static const char* unit_map[]
{
	"Bytes",
	"KiB",
	"MiB",
	"GiB"
};


i8   hmemory_initialize()
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
	total_allocated 		+= size;
	memory_tracker_array[tag] 	+= size;
	return platform_allocate(size,false);
}

void* hmemory_alloc_zeroed(u64 size, memory_tag tag)
{
	void* ret_ptr = hmemory_alloc(size,tag);  // since a heap allocation can't assure a zeroed page it is zeroed out manually
	platform_zero_memory(ret_ptr,size);
	return ret_ptr;
}

void hmemory_free(void* block, memory_tag tag)
{
	u64 size 		  = platform_alloc_size(block);
	total_allocated 	-= size;
	memory_tracker_array[tag] -= size;
	platform_free(block,false);
}

void hlog_memory_report()
{
	HLINFO("SubSystem Memory Allocation Report");
	for(u64 i = 0; i < MEMORY_TAG_COUNT; i++)
	{
		u64 size = memory_tracker_array[i];
		const char* unit = nullptr;
		if( size < BYTESINKiB(1))
		{
			unit = unit_map[0];
		}
		else if( size < BYTESINMiB(1))
		{
			unit = unit_map[1];
		}
		else if( size < BYTESINGiB(1))
		{
			unit = unit_map[2];
		}
		else
		{
			unit = unit_map[3];
		}

		HLINFO(memory_tag_lookup[i],size,unit);
	}
	
}

void hmemory_test(void)
{
	void* mem_ptr[MEMORY_TAG_COUNT][page_slice]{};

	HLINFO("MEMORY TEST INITIAL REPORT\n\n");
	hlog_memory_report();

	for(u32 i = 0; i < MEMORY_TAG_COUNT; i++)
	{
		for(u32 j = 0; j < page_slice; j++)
		{
			mem_ptr[i][j] = hmemory_alloc(single_alloc_cap, static_cast<memory_tag>(i));
		}
	}

	HLINFO("MEMORY TEST ALLOCATION REPORT\n\n");
	hlog_memory_report();

	for( u32 i = 0; i < MEMORY_TAG_COUNT; i++)
	{
		for(u32 j = 0; j < page_slice; j++)
		{
			hmemory_free(mem_ptr[i][j],static_cast<memory_tag>(i));
		}
	}
	
	HLINFO("MEMORY  TEST DEALLOCATION REPORT\n\n");
	hlog_memory_report();

}
