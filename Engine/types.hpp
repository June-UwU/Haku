#pragma once


typedef Status {
    OK = 0x0,
    FAIL = -0x1
} Status;

#define ASSERT_OK(status) if (!isOK(status)) { assert(false && "failed on __FUNCTION__ : __LINE__"); }

inline bool isOK(Status status) {
    return status == OK;
}
