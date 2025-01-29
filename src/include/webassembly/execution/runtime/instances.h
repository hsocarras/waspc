/**
 * @file instances.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef WASPC_WEBASSEMBLY_EXECUTION_RUNTIME_INSTANCES_H
#define WASPC_WEBASSEMBLY_EXECUTION_RUNTIME_INSTANCES_H

#ifdef __cplusplus
    extern "C" {
#endif

#include "webassembly/structure/types.h"
#include "webassembly/execution/runtime/addresses.h"
#include "webassembly/execution/runtime/values.h"
#include "webassembly/structure/module.h"

#include <stdint.h>

typedef struct ModuleInst{

    uint32_t types_len;
    FuncType *types;

    uint32_t globaladdrs_len;
    GlobalAddr *globaladdrs;
    
}ModuleInst;

typedef struct FuncInst{

    FuncType *type;
    ModuleInst *module;
    Func code;

} FuncInst;

typedef struct GlobalInst
{
    uint8_t mut;
    ValType val_type;
    Val value;
}GlobalInst;



#ifdef __cplusplus
    }
#endif

#endif