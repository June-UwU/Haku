/*****************************************************************//**
 * \file   defines.hpp
 * \brief  haku defined general types and macros for easy use
 * 
 * \author June
 * \date   September 2022
 *********************************************************************/
#pragma once
#include <cstdlib>
#include <cstdint>


 /**
 * bit shift macro shifts by x places to the left.
 */
#define BIT(x) 0x1 << x

// HAKU TYPE DEFINES

// TODO : find a more elegant way for runtime assert
/**
 * Haku runtime assertion macro.
 */
#define HASSERT(condition)                                                                                             \
	if (!(condition))                                                                                                    \
	{                                                                                                                  \
		HLEMER("Assertion Failure..!\n File : %s \n Line : %d", __FILE__, __LINE__);                                   \
		abort();                                                                                                       \
	}

// _Static_assert is  a c11 keyword

/**
 * Haku defined static assert.
 */
#define HSTATIC_ASSERT(condition, message) _Static_assert(condition, message)

// objects that are intended to be opaque

/**
* \typedef void typedef
 */
#define opaque							   void

// signed types

/**
* \typedef 8 bit signed value
 */
#define i8								   int8_t

/**
* \typedef 16 bit signed value
*/
#define i16								   int16_t

/**
* \typedef 32 bit signed value
*/
#define i32								   int32_t

/**
* \typedef 64 bit signed value
*/
#define i64								   int64_t

// unsigned type

/**
* \typedef 8 bit unsigned value
*/
#define u8								   uint8_t

/**
* \typedef 16 bit unsigned value
*/
#define u16								   uint16_t

/**
* \typedef 32 bit unsigned value
*/
#define u32								   uint32_t

/**
* \typedef 64 bit unsigned value
*/
#define u64								   uint64_t

// floating point types

/**
* \typedef 32 bit floating point
*/
#define f32								   float

/**
* \typedef 64 bit floating point
*/
#define f64								   double

// HAKU RETURN CODES

/**
 * sucess code.
 */
constexpr i8 H_OK	 = 0;
/**
 * fail code, hard error.
 */
constexpr i8 H_FAIL	 = -1;
/**
 * error code, no critical error.
 */
constexpr i8 H_ERR	 = -2;
/**
 * not an error the application my continue.
 */
constexpr i8 H_NOERR = 1;

/**
 * compile time function to translate KiB into bytes.
 * 
 * \param scale[in] size in KiB to translate to number of bytes
 * \return		number of bytes in the scale value
 */
constexpr u64 BYTESINKiB(u64 scale)
{
	return 1024 * scale;
}

/**
 * compile time function to translate MiB into bytes.
 *
 * \param scale[in] size in MiB to translate to number of bytes
 * \return		number of bytes in the scale value
 */
constexpr u64 BYTESINMiB(u64 scale)
{
	return 1024 * 1024 * scale;
}

/**
 * compile time function to translate GiB into bytes.
 *
 * \param scale[in] size in GiB to translate to number of bytes
 * \return		number of bytes in the scale value
 */
constexpr u64 BYTESINGiB(u64 scale)
{
	return 1024 * 1024 * 1024 * scale;
}

/**
 * compile time function to translate bytes into KiB.
 *
 * \param scale[in] size in bytes to translate to KiB
 * \return		KiB in the scale value
 */
constexpr u64 BYTESTOKiB(u64 bytes)
{
	return bytes / 1024u;
}

/**
 * compile time function to translate bytes into MiB.
 *
 * \param scale[in] size in bytes to translate to MiB
 * \return		MiB in the scale value
 */
constexpr u64 BYTESTOMiB(u64 bytes)
{
	return BYTESTOKiB(bytes) / 1024u;
}

/**
 * compile time function to translate bytes into GiB.
 *
 * \param scale[in] size in bytes to translate to GiB
 * \return		GiB in the scale value
 */
constexpr u64 BYTESTOGiB(u64 bytes)
{
	return BYTESTOMiB(bytes) / 1024u;
}

/**
 * default feild of view angle for the render.
 */
constexpr const f32 DEF_FOV = 90.0f;

/**
 * default height.
 */
constexpr const i32 DEF_HEIGHT = 720;

/**
 * default width.
 */
constexpr const i32 DEF_WIDTH = 1080;

// HAKU API EXPORT MACROS

/** Haku macro for marking a function to be depreciated */
#define HLEGACY __declspec(deprecated)

/**
 * Haku macro for exporting.
 */
#ifdef HAKU_API_BUILD

#define HAPI __declspec(dllexport)

#else

#define HAPI __declspec(dllimport)

#endif
