#pragma once
#include "types.hpp"
#include "logger.hpp"

constexpr const s32 INVALID_VALUE = -1;
#define ASSERT_VALID(index) {\
    [[unlikely]]\
    if(index == INVALID_VALUE) {\
        LOG_CRITICAL("failed on %s : %d",__FUNCTION__,__LINE__);\
        assert(false && "assertion failure");\
    }\
}\

#define ASSERT_OK(status) if (!isOK(status)) { assert(false && "assertion failure"); }

inline bool isOK(Status status) {
    return status == OK;
}
