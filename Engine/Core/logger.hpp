#pragma once

#include "defines.hpp"


#ifdef _DEBUG

// Application unusable state log error

#define HLEMER(message,...) Engine::log(Engine::H_EMER,message,##__VA_ARGS__)

// Application in critical condition, must take action

#define HLCRIT(message,...) Engine::log(Engine::H_CRIT,message,##__VA_ARGS__)

// Application has a non critical error 

#define HLERRO(message,...) Engine::log(Engine::H_ERRO,message,##__VA_ARGS__)

// Application has a non emergency condition that must be addressed

#define HLWARN(message,...) Engine::log(Engine::H_WARN,message,##__VA_ARGS__)

// General information out

#define HLINFO(message,...) Engine::log(Engine::H_INFO,message,##__VA_ARGS__)
#else

// Application unusable state log error

#define HLEMER(message,...) 

// Application in critical condition, must take action

#define HLCRIT(message,...) 

// Application has a non critical error 

#define HLERRO(message,...) 

// Application has a non emergency condition that must be addressed

#define HLWARN(message,...) 

// General information out

#define HLINFO(message,...)

#endif


namespace Engine
{
	typedef enum log_level
	{
		H_EMER, //Application unusable
		H_CRIT, //Critical condition action needs to be made
		H_ERRO, //no critical error condition
		H_WARN, //warning conditions that should taken care of
		H_INFO, // general information messages

		LOG_LVL_COUNT // a enum for log level count
	} log_level;

	//initialization and shutdown
	i32 logger_initiate(void);
	void shutdown(void);

	//logging function
	
	HAPI void log(log_level level,const char* message,...);

};




