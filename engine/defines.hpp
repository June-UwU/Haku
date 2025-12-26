#pragma once
#include <cassert>

#define SUCCESS 1

#define VERIFY(ret_val) assert(SUCCESS == (ret_val))
#define VERIFY_PTR(ptr) assert(nullptr != (ptr))

// Macro to disable copy constructor and copy assignment
#define DISABLE_COPY(name)            \
    name(const name&) = delete;  \
    name& operator=(const name&) = delete

// Macro to disable move constructor and move assignment
#define DISABLE_MOVE(name)             \
    name(name&&) = delete;        \
    name& operator=(name&&) = delete
