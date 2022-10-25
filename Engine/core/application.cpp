/*****************************************************************//**
 * \file   application.cpp
 * \brief  application implementations
 * 
 * \author June
 * \date   September 2022
 *********************************************************************/
#include "event.hpp"
#include "input.hpp"
#include "timer.hpp"
#include "logger.hpp"
#include "application.hpp"
#include "memory/hmemory.hpp"
#include "platform/platform.hpp"
#include "memory/linear_allocator.hpp"
#include "renderer/renderer_front_end.hpp"

#define TEST 0

#if TEST
#include "generics/hash_set.hpp"
#include "generics/single_ll.hpp"
#define RUN_TEST() application_run_test()
#else
#define RUN_TEST()
#endif

/** internal representation for engine state */
typedef enum engine_state
{
	/** engine running */
	RUNNING,

	/** engine suspended */
	SUSPENDED,

	/** applications waiting for shutdown */
	SHUTDOWN
}engine_state;

/** internal engine representation */
typedef struct engine
{
	/** appication window x corrds */
	i32			x; 

	/** appication window y corrds */
	i32			y;

	/** window height */
	u32			height;

	/** window width */
	u32			width;

	/** engine name */
	const char* name;

	/** current engine state */
	engine_state state;

// Subsytems meta data

	/** logger subsystem memory requirement in bytes */
	u64	  logger_mem_require;
	
	/** pointer to the memory for logger in the memory */
	void* logger_state;

	/** platform subsystem memory requirement in bytes */
	u64	  platform_mem_require;

	/** pointer to the memory for platform in the memory */
	void* platform_state;

	/** renderer subsystem memory requirement */
	u64	  renderer_mem_require;

	/** pointer to the memory for renderer in the memory */
	void* renderer_state;
} engine;

/** allocator for subsystem memory */
linear_allocator* allocator; 

/** internal timer */
static timer		 clock; 

/** internal engine state */
static engine* e_state;

/**
 * applications to do a test run.
 */
void application_run_test(void);

/**
 * routine to cleanly exit an applications.
 * 
 * \param sender pointer to the sender of the message
 * \param context return code
 * \return H_OK on sucess and H_FAIL on failure
 */
i8	 application_exit(void* sender, i64 context);

/**
 * routine to resize an applications.
 * 
 * \param sender pointer to the sender
 * \paran context the height and width
 * \return H_OK on sucess and H_FAIL on failure
 */
i8	 application_resize(void* sender, i64 context);

/**
 * routine to suspend the application.
 * 
 * \param sender pointer to the sender
 * \param context UNUSED
 * \return H_OK on sucess and H_FAIL on failure
 */
i8	 application_suspend(void* sender, i64 context);

i8 application_initialize(application_state* app_state)
{

	HLINFO("Hellow >w<");
	i32 haku_ret_code = hmemory_initialize(); 
	if (H_OK != haku_ret_code)
	{
		HLEMER("hmemory subsystem : H_FAIL");
		return H_FAIL;
	}
	allocator = create_linear_allocator(single_alloc_cap);
	HLINFO("Initializing subsystem linear allocator\n \t capacity : %d bytes", allocator->size_in_bytes);
	if (nullptr == allocator)
	{
		HLEMER("linear allocator : H_FAIL");
		return H_FAIL;
	}

	e_state = (engine*)linear_allocate(allocator, sizeof(engine));

	logger_requirement(&e_state->logger_mem_require);

	e_state->logger_state = linear_allocate(allocator, e_state->logger_mem_require);
	haku_ret_code		  = logger_initialize(e_state->logger_state);

	if (H_OK != haku_ret_code)
	{
		HLEMER("logger subsystem : H_FAIL");
		return H_FAIL;
	}

	haku_ret_code = event_initialize();

	if (H_OK != haku_ret_code)
	{
		HLEMER("event subsystem : H_FAIL");
		return H_FAIL;
	}

	platform_requirement(&e_state->platform_mem_require);
	e_state->platform_state = linear_allocate(allocator, e_state->platform_mem_require);

	haku_ret_code = platform_initialize(
		e_state->platform_state, app_state->name, app_state->x, app_state->y, app_state->height, app_state->width);

	if (H_OK != haku_ret_code)
	{
		HLEMER("platform subsystem : H_FAIL");
		return H_FAIL;
	}

	haku_ret_code = input_system_initialize();

	if (H_OK != haku_ret_code)
	{
		HLEMER("input subsystem : H_FAIL");
		return H_FAIL;
	}

	event_register(HK_EXIT, NULL, application_exit);
	event_register(HK_SIZE, NULL, application_resize);
	event_register(HK_MINIMIZE, nullptr, application_suspend);

	renderer_requirement(&e_state->renderer_mem_require);
	e_state->renderer_state = linear_allocate(allocator, e_state->renderer_mem_require);

	haku_ret_code = renderer_initialize(e_state->renderer_state, app_state->width,app_state->height,DEF_FOV,HK_DIRECTX_12);

	if (H_OK != haku_ret_code)
	{
		HLEMER("renderer subsystem : H_FAIL");
		return H_FAIL;
	}

	clock_start(clock);

	e_state->y		   = app_state->y;
	e_state->x		   = app_state->x;
	e_state->width	   = app_state->width;
	e_state->height	   = app_state->height;
	e_state->name	   = app_state->name;
	e_state->state     = RUNNING;
	return haku_ret_code;
}

void application_shutdown(void)
{
	renderer_shutdown();

	event_shutdown();

	input_system_shutdown();

	platform_shutdown();

	logger_shutdown();

	destroy_linear_allocator(allocator);

	hlog_memory_report();

	hmemory_shutdown();
}

void application_run(void)
{
	RUN_TEST();
	HLINFO("Initialization report ");
	hlog_memory_report();
	while (SHUTDOWN != e_state->state)
	{
		clock_update(clock);
		platform_pump_messages();
		service_event();
		input_update(clock.elapsed);

		if (SUSPENDED != e_state->state)
		{
			// TODO : push render_packet outta here
			render_packet pkt{};
			pkt.fov = DEF_FOV;
			pkt.height = e_state->height;
			pkt.width = e_state->width;
			renderer_draw_frame(&pkt);
		}
	}
	clock_stop(clock);
	HLINFO("engine up time : %f", clock.elapsed);
	application_shutdown();
}

i8 application_suspend(void* sender, i64 context)
{
	HLINFO("resize event");
	e_state->state = SUSPENDED;
	return H_OK;
}

void application_run_test(void)
{
#if TEST == 1
	logger_test();
	hmemory_test();
	HLWARN("MEMORY LOG OF BEFORE SLIST");
	hlog_memory_report();
	test_slist();
	HLWARN("MEMORY LOG OF AFTER SLIST");
	hlog_memory_report();

	HLWARN("MEMORY LOG OF BEFORE HASH_TABLE");
	hlog_memory_report();
	test_hash_table();
	HLWARN("MEMORY LOG OF AFTER HASH_TABLE");
	hlog_memory_report();

	HLWARN("MEMORY LOG OF AFTER QUEUE");
	hlog_memory_report();
#endif 
}

i8 application_exit(void* sender, i64 context)
{
	e_state->state = SHUTDOWN;
	return H_OK;
}

i8 application_resize(void* sender, i64 context)
{
	i32 packed_dimensions = LO_DWORD(context);
	e_state->width		  = LO_WORD(packed_dimensions);
	e_state->height		  = HI_WORD(packed_dimensions);
	e_state->state        = RUNNING;
	bool	   is_last = false;
	event_code code	   = event_peek();
	if (HK_SIZE != code)
	{
		is_last = true;
	}
	renderer_resize(e_state->height, e_state->width, is_last);
	return H_OK;
}
