#pragma once
#include <cstdint>
#include <wchar.h>
#include <stdio.h>
#include <numbers>
#ifdef HAKU_API_BUILD
#define HAKU_API __declspec(dllexport)
#else
#define HAKU_API __declspec(dllimport)
#endif
#define KiB								  1024
#define MiB								  KiB * 1024
#define GiB								  MiB * 1024

#define RADS_32BIT(x)					  x*(std::numbers::pi_v<float> / 180.00f)
#define RADS_64BIT(x)					  x*(std::numbers::pi_v<double> / 180.00)

#define DISABLE_COPY_FUNCTION(class_name) class_name(const class_name&) = delete;
#define DISABLE_COPY_OPERATOR(class_name) class_name& operator=(const class_name&) = delete;
#define DISABLE_COPY(class_name)		  DISABLE_COPY_FUNCTION(class_name) DISABLE_COPY_OPERATOR(class_name)

#define DISABLE_MOVE_FUNCTION(class_name) class_name(class_name&&) = delete;
#define DISABLE_MOVE_OPERATOR(class_name) class_name& operator=(class_name&&) = delete;
#define DISABLE_MOVE(class_name)		  DISABLE_MOVE_FUNCTION(class_name) DISABLE_MOVE_OPERATOR(class_name)

#define MAX_ROOT_PARAMETER_COUNT		  64u

namespace math
{
template<typename T>
const T max(const T lhs, const T rhs)
{
	if (lhs > rhs)
	{
		return lhs;
	}
	return rhs;
}

/// DIFFERENT TYPE
template<typename T, typename R>
const T max(const T lhs, const R rhs)
{
	if (lhs > rhs)
	{
		return lhs;
	}
	return rhs;
}

template<typename T>
const T min(const T lhs, const T rhs)
{
	if (lhs < rhs)
	{
		return lhs;
	}
	return rhs;
}

} // namespace math
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

#define HAKU_THROW(error_string)                                                                                       \
	Haku::Errors::Error err(error_string);                                                                             \
	throw err;

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
#define HAKU_SOK_ASSERT_CHAR_PTR_ERROR_BLOB(code, char_ptr)                                                            \
	if (code != S_OK)                                                                                                  \
	{                                                                                                                  \
		std::string line(std::to_string(__LINE__));                                                                    \
		HAKU_LOG_CRIT("FILE: ", __FILE__, " FUNCTION: ", __FUNCTION__, " LINE: ", line);                               \
		OutputDebugStringW(reinterpret_cast<wchar_t*>(char_ptr.Get()));                                                \
		__debugbreak();                                                                                                \
		Haku::Errors::WinError err(code);                                                                              \
		throw err;                                                                                                     \
	}

#define HAKU_DXNAME(DXObjPtr, name) DXObjPtr->SetName(name);
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
#define HAKU_DXNAME(DXObjPtr, name)
#endif
