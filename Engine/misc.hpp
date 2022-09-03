#pragma once

#include "defines.hpp"


template <typename T>
constexpr T hmax(const T lhs, const T rhs)
{
	if(lhs > rhs)
	{
		return lhs;
	}
	return rhs;
}


template <typename T>
constexpr T hmin(const T lhs, const T rhs)
{
	if(lhs > rhs)
	{
		return rhs;
	}
	return lhs;
}
