#ifndef WASPC_RUNTIME_BUILDIN_STD_H
#define WASPC_RUNTIME_BUILDIN_STD_H

#ifdef __cplusplus
    extern "C" {
#endif

#include "interpreter/values.h"

#include <stdint.h>

StackValue SinF64(uint32_t argc,StackValue *args);
StackValue SinF32(uint32_t argc, StackValue *args);

#ifdef __cplusplus
    }
#endif

#endif