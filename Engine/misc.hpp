/*****************************************************************//**
 * \file   misc.hpp
 * \brief  miscellenaous functions
 * 
 * \author June
 * \date   September 2022
 *********************************************************************/
/**
 * \includedoc defines.hpp.
 */
#include "defines.hpp"

/**
 * \brief	std max alternative for haku.
 * 
 * 
 * \param lhs[in]	variable to compare
 * \param rhs[in]	variable to compare
 * \return		the maximum of lhs and rhs
 */	
template<typename T>
constexpr T hmax(const T lhs, const T rhs)
{
	if (lhs > rhs)
	{
		return lhs;
	}
	return rhs;
}

/**
 * \brief	std min alternative for haku.
 * 
 * \param lhs[in]	variable to compare
 * \param rhs[in]	variable to compare
 * \return		minimum of lhs and rhs
 */
template<typename T>
constexpr T hmin(const T lhs, const T rhs)
{
	if (lhs > rhs)
	{
		return rhs;
	}
	return lhs;
}