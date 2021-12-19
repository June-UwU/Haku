#pragma once
#include <cstdint>
#ifdef HAKU_API_BUILD
#define HAKU_API __declspec(dllexport)
#else
#define HAKU_API __declspec(dllimport)
#endif

#define BIT(x) (1 << x)

#define HAKU_BIND_FUNC(x)                                                                                              \
	[this](auto&&... args) -> decltype(auto)                                                                           \
	{                                                                                                                  \
		return this->x(std::forward<decltype(args)>(args)...);                                                         \
	}

#define HAKU_ASSERT(x)                                                                                                 \
	if (!x)                                                                                                            \
	{                                                                                                                  \
		std::string line(std::to_string(__LINE__));                                                                    \
		HAKU_LOG_CRIT("FILE: ", __FILE__, " FUNCTION: ", __FUNCTION__, " LINE: ", line);                               \
		__debugbreak();                                                                                                \
	}

#ifdef _DEBUG
#define HAKU_IF_CHECK_ASSERT(x)                                                                                        \
	if (!x)                                                                                                            \
	{                                                                                                                  \
		std::string line(std::to_string(__LINE__));                                                                    \
		HAKU_LOG_CRIT("FILE: ", __FILE__, " FUNCTION: ", __FUNCTION__, " LINE: ", line);                               \
		__debugbreak();                                                                                                \
		Haku::Errors::Error err("Value assertion error");                                                              \
		throw err;                                                                                                     \
	}

#define HAKU_SOK_ASSERT(x)                                                                                             \
	if (x != S_OK)                                                                                                     \
	{                                                                                                                  \
		std::string line(std::to_string(__LINE__));                                                                    \
		HAKU_LOG_CRIT("FILE: ", __FILE__, " FUNCTION: ", __FUNCTION__, " LINE: ", line);                               \
		__debugbreak();                                                                                                \
		Haku::Errors::WinError err(x);                                                                                 \
		throw err;                                                                                                     \
	}
#else

#define HAKU_IF_CHECK_ASSERT(x)                                                                                        \
	if (!x)                                                                                                            \
	{                                                                                                                  \
		Haku::Errors::Error err("unknown exception");                                                                  \
		throw err;                                                                                                     \
	}
#define HAKU_SOK_ASSERT(x)                                                                                             \
	if (x != S_OK)                                                                                                     \
	{                                                                                                                  \
		Haku::Errors::WinError err(x);                                                                                 \
		throw err;                                                                                                     \
	}
#endif
