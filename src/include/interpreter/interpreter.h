/**
 * @file vm.h
 * @author Hector E. Socarras (hsocarras1987@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef WASPC_INTERPRETER_VM_H
#define WASPC_INTERPRETER_VM_H

#ifdef __cplusplus
    extern "C" {
#endif


#include "webassembly/values.h"

#include <stdint.h>

/**
 * @brief Struct for Virtual machine object
 * 
 */
typedef struct WpInterpreterState{
    
    
    const uint8_t * ip;                                 ///instruction pointer to the current instruction

    WasValue *value_stack;                                  //value stack implemented with     
    WasValue *value_stack_top;                               //pointer to stack's top
    WasValue *value_stack_end;                               //pointer to last element of stack for avoid stack overflow
    
} WpInterpreterState;

/// @brief constructor for vm struct
/// @param  pointer to vm.
void WpInterpreterInit(WpInterpreterState *self);

//void VmFree(VM *);

//Val VmExecuteExpresion(VM *self, const uint8_t * exp);

/// @brief wraper to call VmEvalFrame function. Perform some nit steps.
/// @param  pointer to vm instance.
/// @param  code to execute.
/// @return 
//WpResult VmExecuteFrame(VM *, const uint8_t *);

/// @brief Main evaluaction loop
/// @param  
/// @return 
//WpResult VmEvalFrame (VM *);

//// Stack manipulation functions defined in stack.c //////////////////////////////////////////////////////////////////

/// @brief function to push a value into value stack
/// @param  vm Virtual machine instance.
/// @param  val Value to push.
/// @return bytes pushed into stack.
void PushValue(WpInterpreterState *self, WasValue val);

/// @brief function to pop a value from stack
/// @param  vm Virtual machine instance.
/// @param  val_type Type value to push
/// @return 
WasValue PopValue(WpInterpreterState *self);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Interpreter Eval functions defined in eval.c ///////////////////////////////////////////////////////////////////////
WasValue WpInterpreterEvalExpr(WpInterpreterState *self, const uint8_t *code);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
    }
#endif

#endif