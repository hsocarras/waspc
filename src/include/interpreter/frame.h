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

#include <stdint.h>



typedef struct ActivationFrame {
    const uint8_t *ip;                  // Instruction pointer to the current instruction in the function body
    
    //Value *locals;                      // Pointer to array of local variables (including parameters)
    uint32_t locals_count;              // Total number of locals (params + locals)

    uint32_t arity;                     // Number of return values

    //void *module;                       // Pointer to the module instance (ModuleInst *)
    //void *func;                         // Pointer to the function instance (FuncInst *)

    uint8_t unreachable;               /// unreachable flag
    
} ActivationFrame;



#ifdef __cplusplus
    }
#endif

#endif