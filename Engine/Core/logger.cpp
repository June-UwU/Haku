#include "logger.hpp"

#include <string.h>
#include <stdio.h>
#include <cstdarg>
#include "Platform\platform.hpp"

static i8 initialized = false; // Internal Variable that keeps track of the initialization status
static constexpr u32 OUT_BUFFER_SIZE = 32000u; // internal constexpr that manages the maximum length of log buffer
  

//initialization and shutdown
i32 logger_initiate(void)
{
	initialized = true;		
	return initialized;
}

void shutdown(void)
{

}


//logging function
	
void log(log_level level,const char* message,...)
{
	static const char* log_level_indicator[LOG_LVL_COUNT] //  string map for log level
	{
		"[EMERGENCY]",
		"[CRITICAL]",
		"[ERROR]",
		"[WARN]",
		"[INFO]"
	};

	static char outbuffer[OUT_BUFFER_SIZE]; // declared static and cleared every log
	
	//clear the memory and keep the offset alive
	u32 offset = 0; // offset to current write

	memset(outbuffer,0,OUT_BUFFER_SIZE); 

	i32  indicator_len = strlen(log_level_indicator[level]);

	memcpy(outbuffer,log_level_indicator[level],indicator_len);
	
	offset += indicator_len;

	va_list varadic_list;
	va_start(varadic_list,message);

	vsnprintf((outbuffer + offset), (OUT_BUFFER_SIZE - offset), message, varadic_list);

	va_end(varadic_list);
	
	//  TODO : convert this to platform specfic logging function
	platform_console_write(outbuffer,level);
}


