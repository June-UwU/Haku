#pragma once
#include "types.hpp"
#include "logger.hpp"
#include <vulkan/vulkan.h>

constexpr const s32 INVALID_VALUE = -1;
inline void ASSERT_VALID(s32 index) {
    [[unlikely]] 
    if(index == INVALID_VALUE) {
        assert(false && "failed on __FUNCTION__ : __LINE__");
    }
}

inline void VULKAN_OK(VkResult status, const char* message = ""){
    [[unlikely]] 
    if(status != VK_SUCCESS) {
        assert(false && "failed on __FUNCTION__ : __LINE__"); 
        LOG_FATAL(message);
    }
} 

#define ASSERT_OK(status) if (!isOK(status)) { assert(false && "failed on __FUNCTION__ : __LINE__"); }

inline bool isOK(Status status) {
    return status == OK;
}
