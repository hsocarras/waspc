#ifndef WASPC_RUNTIME_BUILDIN_BUILDIN_H
#define WASPC_RUNTIME_BUILDIN_BUILDIN_H

#ifdef __cplusplus
    extern "C" {
#endif

#include "objects/object.h"
#include "objects/was_deftype.h"
#include "hash_table_host_func.h"
//#include "std.h"

#include <stdint.h>



extern const WpWasDefType signatures[256];

extern const HtHostFuncEntry host_func_entries_std[96];



#ifdef __cplusplus
    }
#endif

#endif