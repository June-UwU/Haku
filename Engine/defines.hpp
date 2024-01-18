#pragma once
#include "types.hpp"
#include "logger.hpp"
#include <vulkan/vulkan.h>

constexpr const s32 INVALID_VALUE = -1;
#define ASSERT_VALID(index) {\
    [[unlikely]]\
    if(index == INVALID_VALUE) {\
        LOG_CRITICAL("failed on %s : %d",__FUNCTION__,__LINE__);\
        assert(false && "assertion failure");\
    }\
}\

#define VULKAN_OK(status,message){\
    [[unlikely]]\
    if(status != VK_SUCCESS) { \
        LOG_CRITICAL("failed on %s : %d",__FUNCTION__,__LINE__);\
        assert(false && "assertion failure");\
        LOG_FATAL(message);\
    }\
}\

#define ASSERT_OK(status) if (!isOK(status)) { assert(false && "assertion failure"); }

inline bool isOK(Status status) {
    return status == OK;
}
