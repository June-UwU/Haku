#pragma once
#include <cassert>

#define SUCCESS 1

#define VERIFY(ret_val) assert(SUCCESS == (ret_val)) 
#define VERIFY_PTR(ptr) assert(nullptr != (ptr))
