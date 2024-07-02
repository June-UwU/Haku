#pragma once
#include "logger.hpp"

#if defined(NDEBUG)

#define ASSERT(predicate, format)                           \
	if (!(predicate)) {                                     \
		ERROR << __FUNCTION__ << " : " << __LINE__ << "\n"; \
		ERROR << format << "\n";                            \
		exit(EXIT_FAILURE);                                 \
	}

#define ASSERT_UNREACHABLE()                                                          \
	FATAL << "unreachable code segment reached.!!!\n"                                 \
		  << " : " << __FILE__ << " : " << __FUNCTION__ << " : " << __LINE__ << "\n"; \
	exit(EXIT_FAILURE);

#define NOT_IMPLEMENTED(format)                            \
	FATAL << "feature is not yet implemented\n"            \
		  << __FILE__ << __FUNCTION__ << __LINE__ << "\n"; \
	FATAL << format << "\n";                               \
	exit(EXIT_FAILURE);

#define TODO(format) \
	WARN << "TODO : " << __FUNCTION__ << " \nmessage : " << format << "\n";
#else

#if defined(_WIN32) || defined(_WIN64)
#define debug_break() __debugbreak()
#else
#define debug_break() __builtin_trap()
#endif

#define ASSERT(predicate, format)                           \
	if (!(predicate)) {                                     \
		ERROR << __FUNCTION__ << " : " << __LINE__ << "\n"; \
		ERROR << format << "\n";                            \
		debug_break();                                      \
		exit(EXIT_FAILURE);                                 \
	}

#define ASSERT_UNREACHABLE()                                                          \
	FATAL << "unreachable code segment reached.!!!\n"                                 \
		  << " : " << __FILE__ << " : " << __FUNCTION__ << " : " << __LINE__ << "\n"; \
	debug_break();                                                                    \
	exit(EXIT_FAILURE);

#define NOT_IMPLEMENTED(format)                            \
	FATAL << "feature is not yet implemented\n"            \
		  << __FILE__ << __FUNCTION__ << __LINE__ << "\n"; \
	FATAL << format << "\n";                               \
	debug_break();                                         \
	exit(EXIT_FAILURE);

#define TODO(format) \
	WARN << "TODO : " << __FUNCTION__ << " \nmessage : " << format << "\n";

#endif