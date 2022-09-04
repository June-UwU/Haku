#include "logger.hpp"

#include <string.h>
#include <stdio.h>
#include <cstdarg>
#include "platform\platform.hpp"

  
typedef struct logger_state
{
	i8 initialized = false; // Internal Variable that keeps track of the initialization status
}logger_state;

static logger_state* log_state;

void logger_requirement(u64* memory_requirement)
{
	*memory_requirement = sizeof(logger_state);
}

//initialization and shutdown
i8 logger_initialize(void* state)
{
	if (nullptr == state)
	{
		HLEMER("memory allocation failure");
		return H_FAIL;
	}
	log_state = (logger_state*)state;
	log_state->initialized = true;
	return H_OK;
}

void logger_shutdown(void)
{
	log_state = 0;
}


//logging function
	
void log(log_level level,const char* message,...)
{
	static char outbuffer[OUT_BUFFER_SIZE]; // declared static and cleared every log
	
	//clear the memory and keep the offset alive
	u32 offset = 0; // offset to current write

	platform_set_memory(outbuffer,0, OUT_BUFFER_SIZE);

	i32  indicator_len = strlen(log_level_indicator[level]);

	platform_copy_memory(outbuffer,(void*)log_level_indicator[level],indicator_len);
	
	offset += indicator_len;

	va_list varadic_list;
	va_start(varadic_list,message);

	vsnprintf((outbuffer + offset), (OUT_BUFFER_SIZE - offset), message, varadic_list);

	va_end(varadic_list);
	
	platform_console_write(outbuffer,level);
}

void logger_test(void)
{
	HLEMER("Emergency %d", 1);
	HLCRIT("Critical %d", 2);
	HLERRO("Error %d", 3);
	HLWARN("Warning %d", 4);
	HLINFO("Information %d", 5);
}
