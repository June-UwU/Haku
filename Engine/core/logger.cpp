#include "logger.hpp"
#include <cstring>
#include <stdio.h>
#include <stdarg.h>
#include <unistd.h>

constexpr s32 LOGGER_BUFFER_SIZE = 512;
static char LOG_BUFFER[LOGGER_BUFFER_SIZE];

const Status initializeLogger(void)
{
  memset(LOG_BUFFER,0,LOGGER_BUFFER_SIZE);
  return OK;
}


void shutdownLogger(void)
{
}

void log(log_level lvl,const char* message, ...)
{
  u32 log_level_len = strlen(LOG_LEVEL_STR[lvl]);
  memcpy(LOG_BUFFER,LOG_LEVEL_STR[lvl],log_level_len);
  va_list arg;
  va_start(arg,message);
  log_level_len += vsnprintf((LOG_BUFFER + log_level_len),(LOGGER_BUFFER_SIZE - log_level_len - 1),message,arg);
  va_end(arg);
  
  memcpy(LOG_BUFFER + log_level_len, "\n\0",2);

  write(STDOUT_FILENO,LOG_BUFFER,log_level_len + 2);
}
