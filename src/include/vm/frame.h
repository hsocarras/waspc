/**
 * @file frame.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-25
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef WASPC_VM_FRAME_H
#define WASPC_VM_FRAME_H

#ifdef __cplusplus
    extern "C" {
#endif

#include "vm/values.h"
#include "webassembly/execution/runtime/instances.h"

#include <stdint.h>



typedef struct ActivationFrame{

    uint32_t arity;                     //number of return values
    VmValue *locals;                   //array to pointers to locals inside stack
    FuncInst *func;
} ActivationFrame;



#ifdef __cplusplus
    }
#endif

#endif