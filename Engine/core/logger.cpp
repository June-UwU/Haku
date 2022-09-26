/*****************************************************************//**
 * \file   logger.cpp
 * \brief  logger implementation
 * 
 * \author June
 * \date   September 2022
 *********************************************************************/
#include "logger.hpp"

#include <string.h>
#include <stdio.h>
#include <cstdarg>
#include "core/file_system.hpp"
#include "platform/platform.hpp"

#include "memory/hmemory.hpp"


/** logger state */
typedef struct logger_state
{
	/** file pointer to logger */
	file* logger_file;

	/** buffer to hold logger  */
} logger_state;

/**  variable that tracks the logger initiailzations */
static bool initialized = false;

/** internal variable for logger tracking */
static logger_state* log_state;

/** logger buffer  */
static char buffer[LOGGER_BUFFER_SIZE];

/**
 * routine to write to log buffer.
 * 
 * \param message pointer to message
 */
void logger_file_write(char* message);

void logger_requirement(u64* memory_requirement)
{
	*memory_requirement = sizeof(logger_state);
}

i8 logger_initialize(void* state)
{
	if (nullptr == state)
	{
		HLEMER("memory allocation failure");
		return H_FAIL;
	}
	log_state			   = (logger_state*)state;
	log_state->logger_file = file_open("Haku.log", WRITE);
	initialized = true;
	return H_OK;
}

void logger_shutdown(void)
{
	initialized = false;
	file_close(log_state->logger_file);
	log_state = 0;
}

// logging function

void log(log_level level, const char* message, ...)
{
	// clear the memory and keep the offset alive
	u32 offset = 0; // offset to current write

	platform_set_memory(buffer, 0, LOGGER_BUFFER_SIZE);

	i32 indicator_len = strlen(log_level_indicator[level]);

	platform_copy_memory(buffer, (void*)log_level_indicator[level], indicator_len);

	offset += indicator_len;

	va_list varadic_list;
	va_start(varadic_list, message);

	vsnprintf((buffer + offset), (LOGGER_BUFFER_SIZE - offset - 1), message, varadic_list);

	va_end(varadic_list);

	platform_console_write(buffer, level);
	if (nullptr != log_state)
	{
		logger_file_write(buffer);
	}
}

void logger_test(void)
{
	HLEMER("Emergency %d", 1);
	HLCRIT("Critical %d", 2);
	HLERRO("Error %d", 3);
	HLWARN("Warning %d", 4);
	HLINFO("Information %d", 5);
}

void logger_file_write(char* message)
{
	if (log_state->logger_file && true == initialized)
	{
		u64 size	  = strlen(message);
		message[size] = '\n';
		file_write(log_state->logger_file, sizeof(char), size  + 1, message);
	}
}
