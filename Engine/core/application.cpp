#include "event.hpp"
#include "input.hpp"
#include "timer.hpp"
#include "logger.hpp"
#include "application.hpp"
#include "memory/hmemory.hpp"
#include "platform/platform.hpp"

#include "renderer/renderer_front_end.hpp"

#define TEST 0

#if TEST
	#define RUN_TEST() application_run_test()
#else
	#define RUN_TEST()
#endif

// internal engine representation
typedef struct engine_state
{
	i32 x;			// appication window corrds
	i32 y;
	u32 height;		// window height
	u32 width;		// window width
	const char* name;	// engine name
	bool running;		// true if state is running
	bool suspended;		// true is suspended
}engine_state;


static timer clock;	    // internal timer
static engine_state e_state;

// private events
void application_run_test(void);
i8 application_exit(void* sender, i64 context);
i8 application_resize(void* sender, i64 context);
i8 application_suspend(void* sender, i64 context);

void* platform_state = nullptr; // store platform dependant data

i8 application_initialize(application_state* app_state)
{
	HLINFO("Hellow >w<");

	i32 haku_ret_code = hmemory_initialize(); // memory initialize
	
	if( H_OK != haku_ret_code)
	{
		HLEMER("hmemory subsystem : H_FAIL");
		return H_FAIL;
	}

	haku_ret_code = logger_initialize();

	if( H_OK != haku_ret_code )
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


	haku_ret_code = platform_initialize(platform_state , app_state->name , app_state->x , app_state->y , app_state->height , app_state->width);

	if( H_OK != haku_ret_code )
	{
		HLEMER("platform subsystem : H_FAIL");
		return H_FAIL;
	}

	haku_ret_code = input_system_initialize();

	if( H_OK != haku_ret_code)
	{
		HLEMER("input subsystem : H_FAIL");
		return H_FAIL;
	}


	event_register(HK_EXIT,NULL,application_exit);
	event_register(HK_SIZE, NULL, application_resize);
	event_register(HK_MINIMIZE, nullptr, application_suspend);
	haku_ret_code	= renderer_initialize(HK_DIRECTX_12);

	if( H_OK != haku_ret_code)
	{
		HLEMER("renderer subsystem : H_FAIL");
		return H_FAIL;
	}

	clock_start(clock);
	
	e_state.suspended	= false;
	e_state.y		= app_state->y;
	e_state.x		= app_state->x;
	e_state.width		= app_state->width;
	e_state.height		= app_state->height;
	e_state.name	 	= app_state->name;
	e_state.running		= true;

	return haku_ret_code;
}

void application_shutdown(void)
{
	renderer_shutdown();

	event_shutdown();

	input_system_shutdown();

	platform_shutdown();

	hlog_memory_report();

	logger_shutdown();

	hmemory_shutdown();
}

void application_run(void)
{
	RUN_TEST();
	HLINFO("Initialization report ");
	hlog_memory_report();
	while(true == e_state.running)
	{
		clock_update(clock);
		platform_pump_messages();
		service_event();	
		input_update(clock.elapsed);

		if (false == e_state.suspended)
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
	e_state.suspended = true;
	return H_OK;
}

void application_run_test(void)
{
	logger_test();
	hmemory_test();
}

i8 application_exit(void* sender, i64 context)
{
	e_state.running = false;
	return H_OK;
}

i8 application_resize(void* sender, i64 context)
{
	i32 packed_dimensions = LO_DWORD(context);
	e_state.width = LO_WORD(packed_dimensions);
	e_state.height = HI_WORD(packed_dimensions);
	
	bool is_last = false;
	event_code code = event_peek();
	if (HK_SIZE != code)
	{
		is_last = true;
	}
	renderer_resize(e_state.height, e_state.width, is_last);
	return H_OK;
}