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

#include "webassembly/execution/runtime/values.h"

#include <stdint.h>



typedef struct ActivationFrame {
    uint32_t arity;                     // Number of return values
    Value *locals;                      // Pointer to array of local variables (including parameters)
    void *module;                       // Pointer to the module instance (ModuleInst *)
    void *func;                         // Pointer to the function instance (FuncInst *)
    uint32_t locals_count;              // Total number of locals (params + locals)
} ActivationFrame;



#ifdef __cplusplus
    }
#endif

#endif