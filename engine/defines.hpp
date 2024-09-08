#pragma once
#include "logger.hpp"
#include <vector>

#define HAKU_UNUSED(var) (void)(var)

template <typename T>
void print_vec(std::vector<T>& vec) {
	TRACE << "{";
	for (auto val : vec) {
		TRACE << val << ", ";
	}
	TRACE << "}\n";
}

#if defined(NDEBUG)

#if defined(_WIN32) || defined(_WIN64)
#define debug_break() exit(EXIT_FAILURE)
#else
#define debug_break() exit(EXIT_FAILURE)
#endif

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
	WARN << "TODO : " << __FUNCTION__ << " : " << format << "\n";

#define HAKU_FATAL_CRASH() exit(EXIT_FAILURE)
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
	WARN << "TODO : " << __FUNCTION__ << " : " << format << "\n";

#define HAKU_FATAL_CRASH() debug_break()
#endif
