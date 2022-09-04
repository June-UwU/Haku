#pragma once

#include "defines.hpp"


// Haku logger subsystem, 
//
// Usage : ideally called by macros that may translate to the generic log function for the purpose needed , log function is not intended to be directly called
	
	typedef enum log_level
	{
		H_EMER, //Application unusable
		H_CRIT, //Critical condition action needs to be made
		H_ERRO, //no critical error condition
		H_WARN, //warning conditions that should taken care of
		H_INFO, // general information messages

		LOG_LVL_COUNT // a enum for log level count
	} log_level;

	constexpr u32 OUT_BUFFER_SIZE =  32000u; // internal constexpr that manages the maximum length of log buffer

	constexpr const char* log_level_indicator[LOG_LVL_COUNT] //  string map for log level
	{
		"[EMERGENCY]",
		"[CRITICAL]",
		"[ERROR]",
		"[WARN]",
		"[INFO]"
	};

	void logger_requirement(u64* memory_requirement);

	//initialization and shutdown
	i8   logger_initialize(void* state);
	void logger_shutdown(void);

	//logging function : DONOT CALL THIS DIRECTLY IN ANY PART
	
	HAPI void log(log_level level,const char* message,...);

	// @breif	logger subsystem test routine
	void logger_test(void);



#ifdef _DEBUG

// Application unusable state log error

#define HLEMER(message,...) log(H_EMER,message,##__VA_ARGS__)

// Application in critical condition, must take action

#define HLCRIT(message,...) log(H_CRIT,message,##__VA_ARGS__)

// Application has a non critical error 

#define HLERRO(message,...) log(H_ERRO,message,##__VA_ARGS__)

// Application has a non emergency condition that must be addressed

#define HLWARN(message,...) log(H_WARN,message,##__VA_ARGS__)

// General information out

#define HLINFO(message,...) log(H_INFO,message,##__VA_ARGS__)

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

