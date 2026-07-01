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

#ifndef WASPC_INTERPRETER_FRAME_H
#define WASPC_INTERPRETER_FRAME_H

#ifdef __cplusplus
    extern "C" {
#endif
#include "interpreter/values.h"
#include "objects/module_state.h"
#include "objects/module_instance.h"

#include <stdint.h>




typedef struct CallFrame {
    const uint8_t *blocks;                    // pointer to first block of the frame, used for control flow instructions
    const uint8_t *block_top;                 // pointer to the current block of the frame, used for control flow instructions
    
    StackValue *locals;                       // base pointer (where the frame start on the stack)
    uint32_t locals_count;              // Total number of locals (params + locals)

    uint32_t arity;                     // Number of return values
    const uint8_t *ip;                    // instruction pointer for the current execution point in the function body

    WpModuleInstance *module;              // Pointer to the module instance (ModuleInst *)
    //void *func;                         // Pointer to the function instance (FuncInst *)

    //uint8_t unreachable;               /// unreachable flag
    
} CallFrame;



#ifdef __cplusplus
    }
#endif

#endif