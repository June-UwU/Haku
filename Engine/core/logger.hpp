/*****************************************************************//**
 * \file   logger.hpp
 * \brief  haku logger functions
 * 
 * \author June
 * \date   September 2022
 *********************************************************************/
#pragma once

#include "defines.hpp"

// Usage : ideally called by macros that may translate to the generic log function for the purpose needed , log function
// is not intended to be directly called

/**
 * logger level for logs and color coded for severity.
 */
typedef enum log_level
{
	/** Application unusable */
	H_EMER,

	/** Critical condition action needs to be made */
	H_CRIT, 

	/** no critical error condition */
	H_ERRO, 

	/** warning conditions that should taken care of */
	H_WARN, 

	/** general information messages */
	H_INFO, 

	/** a enum for log level count */
	LOG_LVL_COUNT 
} log_level;

/** internal constexpr that manages the maximum length of log buffer */
constexpr u32 LOGGER_BUFFER_SIZE = BYTESINKiB(64); // 

/** string map for log level */
constexpr const char* log_level_indicator[LOG_LVL_COUNT] 
	{ "[EMERGENCY]", "[CRITICAL]", "[ERROR]", "[WARN]", "[INFO]" };

/**
 * routine to get the memory requirement.
 * 
 * \param memory_requirement memory in bytes needed
 */
void logger_requirement(u64* memory_requirement);

/**
 * initialization routine.
 * 
 * \param state memory allocated for the subsystem for functions
 * \return H_OK on sucess and H_FAIL on failure
 */
i8	 logger_initialize(void* state);

/**
 * routine to properly shutdown.
 */
void logger_shutdown(void);

// logging function : DONOT CALL THIS DIRECTLY IN ANY PART

/**
* routine to log messages to console and file.
* 
* \param	  level level of severity of message
* \param	  message format message with varadic args
*/
HAPI void log(log_level level, const char* message, ...);

/**  logger subsystem test routine */
void logger_test(void);

#ifdef _DEBUG

/**
 * Application unusable state log error.
 * 
 * \param message format message for the logger and varaidic args
 */
#define HLEMER(message, ...) log(H_EMER, message, ##__VA_ARGS__)

/**
 * Application in critical condition, must take action.
 *
 * \param message format message for the logger and varaidic args
 */
#define HLCRIT(message, ...) log(H_CRIT, message, ##__VA_ARGS__)

/**
 * Application has a non critical error.
 *
 * \param message format message for the logger and varaidic args
 */
#define HLERRO(message, ...) log(H_ERRO, message, ##__VA_ARGS__)

/**
 * Application has a non emergency condition that must be addressed.
 *
 * \param message format message for the logger and varaidic args
 */
#define HLWARN(message, ...) log(H_WARN, message, ##__VA_ARGS__)

/**
 * General information out.
 *
 * \param message format message for the logger and varaidic args
 */
#define HLINFO(message, ...) log(H_INFO, message, ##__VA_ARGS__)

#else

/**
 * Application unusable state log error.
 *
 * \param message format message for the logger and varaidic args
 */
#define HLEMER(message, ...) 

 /**
  * Application in critical condition, must take action.
  *
  * \param message format message for the logger and varaidic args
  */
#define HLCRIT(message, ...) 

  /**
   * Application has a non critical error.
   *
   * \param message format message for the logger and varaidic args
   */
#define HLERRO(message, ...) 

   /**
	* Application has a non emergency condition that must be addressed.
	*
	* \param message format message for the logger and varaidic args
	*/
#define HLWARN(message, ...) 

	/**
	 * General information out.
	 *
	 * \param message format message for the logger and varaidic args
	 */
#define HLINFO(message, ...)

#endif
