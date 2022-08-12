#include "logger.hpp"

#include <string.h>
#include <stdio.h>
#include <cstdarg>

namespace Engine
{

	static i8 initialized = false;
	static constexpr u32 OUT_BUFFER_SIZE = 32000u;

	  
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
		static const char* log_level_indicator[LOG_LVL_COUNT]
		{
			"[EMERGENCY]",
			"[CRITICAL]",
			"[ERROR]",
			"[WARN]",
			"[INFO]"
		};

		static char outbuffer[OUT_BUFFER_SIZE];
		
		//clear the memory and keep the offset alive
		u32 offset = 0;
	
		memset(outbuffer,0,OUT_BUFFER_SIZE);
	
		i32  indicator_len = strlen(log_level_indicator[level]);

		memcpy(outbuffer,log_level_indicator[level],indicator_len);
		
		offset += indicator_len;

		va_list varadic_list;
		va_start(varadic_list,message);

		vsnprintf((outbuffer + offset),(OUT_BUFFER_SIZE - offset),message,varadic_list);

		va_end(varadic_list);

		printf("%s\n",outbuffer);
	}


};
