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
#include "generics/queue.hpp"
#include "generics/hash_set.hpp"
#include "generics/single_ll.hpp"
#define RUN_TEST() application_run_test()
#else
#define RUN_TEST()
#endif

// internal engine representation
typedef struct engine_state
{
	i32			x; // appication window corrds
	i32			y;
	u32			height;	   // window height
	u32			width;	   // window width
	const char* name;	   // engine name
	bool		running;   // true if state is running
	bool		suspended; // true is suspended

	// Subsytems meta data
	u64	  logger_mem_require;
	void* logger_state;

	u64	  platform_mem_require;
	void* platform_state;

	u64	  renderer_mem_require;
	void* renderer_state;
} engine_state;

linear_allocator* allocator; // allocator for subsystem memory

static timer		 clock; // internal timer
static engine_state* e_state;

// private events
void application_run_test(void);
i8	 application_exit(void* sender, i64 context);
i8	 application_resize(void* sender, i64 context);
i8	 application_suspend(void* sender, i64 context);

i8 application_initialize(application_state* app_state)
{

	i32 haku_ret_code = hmemory_initialize(); // memory initialize

	if (H_OK != haku_ret_code)
	{
		//HLEMER("hmemory subsystem : H_FAIL");
		return H_FAIL;
	}

	allocator = create_linear_allocator(single_alloc_cap);

	if (nullptr == allocator)
	{
		//HLEMER("linear allocator : H_FAIL");
		return H_FAIL;
	}

	e_state = (engine_state*)linear_allocate(allocator, sizeof(engine_state));

	logger_requirement(&e_state->logger_mem_require);

	e_state->logger_state = linear_allocate(allocator, e_state->logger_mem_require);
	haku_ret_code		  = logger_initialize(e_state->logger_state);
	HLINFO("Hellow >w<");

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

	haku_ret_code = renderer_initialize(e_state->renderer_state, HK_DIRECTX_12);

	if (H_OK != haku_ret_code)
	{
		HLEMER("renderer subsystem : H_FAIL");
		return H_FAIL;
	}

	clock_start(clock);

	e_state->suspended = false;
	e_state->y		   = app_state->y;
	e_state->x		   = app_state->x;
	e_state->width	   = app_state->width;
	e_state->height	   = app_state->height;
	e_state->name	   = app_state->name;
	e_state->running   = true;

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
	while (true == e_state->running)
	{
		clock_update(clock);
		platform_pump_messages();
		service_event();
		input_update(clock.elapsed);

		if (false == e_state->suspended)
		{
			// TODO : push render_packet outta here
			render_packet packet{};
			renderer_draw_frame(&packet);
		}
	}
	clock_stop(clock);
	HLINFO("engine up time : %f", clock.elapsed);
	application_shutdown();
}

i8 application_suspend(void* sender, i64 context)
{
	HLINFO("resize event");
	e_state->suspended = true;
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

	HLWARN("MEMORY LOG OF BEFORE QUEUE");
	hlog_memory_report();
	test_queue();
	HLWARN("MEMORY LOG OF AFTER QUEUE");
	hlog_memory_report();
#endif 
}

i8 application_exit(void* sender, i64 context)
{
	e_state->running = false;
	return H_OK;
}

i8 application_resize(void* sender, i64 context)
{
	i32 packed_dimensions = LO_DWORD(context);
	e_state->width		  = LO_WORD(packed_dimensions);
	e_state->height		  = HI_WORD(packed_dimensions);

	bool	   is_last = false;
	event_code code	   = event_peek();
	if (HK_SIZE != code)
	{
		is_last = true;
	}
	renderer_resize(e_state->height, e_state->width, is_last);
	return H_OK;
}
