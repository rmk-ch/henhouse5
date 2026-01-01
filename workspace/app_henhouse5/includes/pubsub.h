#pragma once
#include "errorcode.h"

typedef void (static_fcn_uint32)(void*, uint32_t);

struct callback_entry {
    static_fcn_uint32* func_ptr;
    void * instance;
};
