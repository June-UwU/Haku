#pragma once
#include <cassert>
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

/*******************************RETURN STATUS**************************/
typedef enum Status {
    OK = 0x0,
    FAIL = -0x1
} Status;
