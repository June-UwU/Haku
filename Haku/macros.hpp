#pragma once
#include <cstdint>
#include <numbers>
#ifdef HAKU_API_BUILD
#define HAKU_API __declspec(dllexport)
#else
#define HAKU_API __declspec(dllimport)
#endif
#define KiB			  1024
#define MiB			  KiB * 1024
#define GiB			  MiB * 1024

#define RADS_32BIT(x) x*(std::numbers::pi_v<float> / 180.00f)
#define RADS_64BIT(x) x*(std::numbers::pi_v<double> / 180.00)

const int FrameCount{ 3 };

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

#define HAKU_SOK_ASSERT(code)                                                                                          \
	if (code != S_OK)                                                                                                  \
	{                                                                                                                  \
		std::string line(std::to_string(__LINE__));                                                                    \
		HAKU_LOG_CRIT("FILE: ", __FILE__, " FUNCTION: ", __FUNCTION__, " LINE: ", line);                               \
		__debugbreak();                                                                                                \
		Haku::Errors::WinError err(code);                                                                              \
		throw err;                                                                                                     \
	}
#define HAKU_SOK_ASSERT_CHAR_PTR(code, char_ptr)                                                                       \
	if (code != S_OK)                                                                                                     \
	{                                                                                                                  \
		std::string line(std::to_string(__LINE__));                                                                    \
		HAKU_LOG_CRIT("FILE: ", __FILE__, " FUNCTION: ", __FUNCTION__, " LINE: ", line);                               \
		OutputDebugStringW((wchar_t*)char_ptr.Get());                                                                             \
		__debugbreak();                                                                                                \
		Haku::Errors::WinError err(code);                                                                                 \
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
#define HAKU_SOK_ASSERT_CHAR_PTR(code, char_ptr)                                                                       \
	if (code != S_OK)                                                                                                  \
	{                                                                                                                  \
		std::string			   line(std::to_string(__LINE__));                                                         \
		Haku::Errors::WinError err(code);                                                                              \
		throw err;                                                                                                     \
	}
#endif
