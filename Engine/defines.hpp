#pragma once
#include <cstdint>
#include <cstdlib>

/*********************************SIGNED TYPES****************************************/
typedef   int8_t    s8;
typedef   int16_t   s16;
typedef   int32_t   s32;
typedef   int64_t   s64;

/*********************************UNSIGNED TYPES**************************************/
typedef   uint8_t   u8;
typedef   uint16_t  u16;
typedef   uint32_t  u32;
typedef   uint64_t  u64;

/*******************************UNKNOWN OR TYPE ERASURE TYPE**************************/
typedef   void*   handle;

/*******************************INTERNAL HAKU TYPES***********************************/
typedef   void*   HakuWindow;


/*******************************HAKU RETURN VALUES************************************/

constexpr   s8  H_OK    = 0x0;
constexpr   s8  H_FAIL  = -0x1;
