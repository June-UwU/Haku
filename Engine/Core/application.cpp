#include "logger.hpp"
#include "hmemory.hpp"
#include "application.hpp"
#include "platform/platform.hpp"

#define TEST 1

#if TEST
	#define RUN_TEST() application_run_test()
#else
	#define RUN_TEST()
#endif

void application_run_test(void)
{
	logger_test();
	hmemory_test();
}


void* platform_state = nullptr; // store platform dependant data

i8 application_initialize(application_state* app_state)
{
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

	return H_OK;
}

void application_shutdown(void)
{
	platform_shutdown();

	logger_shutdown();

	hmemory_shutdown();
}


void application_run(void)
{
	RUN_TEST();
	while(true)
	{
		platform_pump_messages();		
	}
	application_shutdown();
}

