#pragma once
#include "types.hpp"

typedef enum
{
  TRACE = 0x0,
  WARN,
  CRITICAL,
  FATAL,
  LOG_LEVEL_MAX
}log_level;

constexpr const char* LOG_LEVEL_STR[LOG_LEVEL_MAX] = 
{ 
  "\033[32m TRACE : " ,
  "\033[33m WARN : ", 
  "\033[31m CRITICAL : ", 
  "\033[1;31m FATAL : "
};

[[nodiscard]] Status initializeLogger(void);
void log(log_level lvl,const char* message,...);
void shutdownLogger(void);


#define LOG_TRACE(message,...)        log(TRACE,message,##__VA_ARGS__)
#define LOG_WARN(message,...)         log(WARN,message,##__VA_ARGS__)
#define LOG_CRITICAL(message,...)     log(CRITICAL,message,##__VA_ARGS__)
#define LOG_FATAL(message,...)        log(FATAL,message,##__VA_ARGS__)
