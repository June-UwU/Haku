#include "event.hpp"
#include "input.hpp"
#include "timer.hpp"
#include "logger.hpp"
#include "hmemory.hpp"
#include "application.hpp"
#include "platform/platform.hpp"


#define TEST 0

#if TEST
	#define RUN_TEST() application_run_test()
#else
	#define RUN_TEST()
#endif

static timer app_timer;
static bool running = false;

void application_run_test(void)
{
	logger_test();
	hmemory_test();
}

i8 application_exit(void* sender, i64 context)
{
	running 	= false;
	return H_OK;
}

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

	haku_ret_code	 = event_initialize();

	if (H_OK != haku_ret_code)
	{
		HLEMER("event subsystem : H_FAIL");
		return H_FAIL;
	}

	event_register(HK_EXIT,NULL,application_exit);

	clock_start(app_timer);

	running	= true;

	return H_OK;
}

void application_shutdown(void)
{
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
	while(true == running)
	{
		clock_update(app_timer);
		platform_pump_messages();
		service_event();	
		input_update(app_timer.elapsed);	
	}
	application_shutdown();
	clock_stop(app_timer);
}

