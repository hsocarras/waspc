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
#include "webassembly/values.h"
#include "objects/module.h"

#include <stdint.h>



typedef struct CallFrame {
    //const uint8_t *ip;                  // Instruction pointer to the current instruction in the function body
    
    WasValue *bp;                       // base pointer (where the frame start on the stack)
    uint32_t locals_count;              // Total number of locals (params + locals)

    uint32_t arity;                     // Number of return values

    WpModuleState *module;              // Pointer to the module instance (ModuleInst *)
    //void *func;                         // Pointer to the function instance (FuncInst *)

    //uint8_t unreachable;               /// unreachable flag
    
} CallFrame;



#ifdef __cplusplus
    }
#endif

#endif