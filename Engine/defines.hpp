#pragma once
#include "types.hpp"
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


#define VULKAN_OK(status,message) if(status != VK_SUCCESS) { assert(false && "failed on __FUNCTION__ : __LINE__"); LOG_FATAL(message);}