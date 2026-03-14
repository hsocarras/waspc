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


#include "interpreter/ctrl_frame.h"
#include "interpreter/values.h"

struct WpStore;  //fordward declaration

#include <stdint.h>

/**
 * @brief Struct for Virtual machine object
 * 
 */
typedef struct WpInterpreterState{
    
    
    const uint8_t * ip;                                 ///instruction pointer to the current instruction

    StackValue *value_stack;                              //value stack implemented with     
    StackValue *value_stack_top;                               //pointer to stack's top
    StackValue *value_stack_end;                               //pointer to last element of stack for avoid stack overflow

    CtrlFrame ctrl_satck[256];                          //TODO size asignation
    uint32_t ctrl_count;
    
    struct WpStore *store;
    
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
void PushValue(WpInterpreterState *self, StackValue val);

/// @brief function to pop a value from stack
/// @param  vm Virtual machine instance.
/// @param  val_type Type value to push
/// @return 
StackValue PopValue(WpInterpreterState *self);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Interpreter Eval functions defined in eval.c ///////////////////////////////////////////////////////////////////////
StackValue WpInterpreterEvalExpr(WpInterpreterState *self, const uint8_t *code);

uint8_t WpInterpreterExecuteCallRefFunc(WpInterpreterState *self, const uint8_t *type);
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
    }
#endif

#endif