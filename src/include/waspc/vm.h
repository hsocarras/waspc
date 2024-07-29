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

#ifndef AOS_VM_VM_H
#define AOS_VM_VM_H

#ifdef __cplusplus
    extern "C" {
#endif

#include "vm/frame.h"
#include "vm/values.h"

#include <stdint.h>

#define VM_VALUE_STACK_SIZE 512       //later must see how to include the PortConfig.h with Cmake and a target variable

#ifndef VM_VALUE_STACK_SIZE
        #error Missing definition:  VM_MAX_STACK_SIZE must be defined in PortConfig.h.
#endif

#define VM_CALL_STACK_SIZE = 24       //must not be greater than 255





typedef enum {
    INTERPRET_OK,
    INTERPRET_RUNTIME_ERROR
} InterpreterResult;



/**
 * @brief Struct for Virtual machine object
 * 
 */
typedef struct {
    
    //uint8_t *ip;                                      /// instruction pointer   
    uint8_t frame_count;                                //index in call stack
    Value *stack_top;                                   /// stack pointer
    Value value_stack[VM_VALUE_STACK_SIZE];                /// Vm's value stack and base pointer 
    Frame call_stack[VM_CALL_STACK_SIZE];
    
    
}VM;

//In vm.c////////////////////////////////////////////
void push(VM *, Value);
Value pop(VM *);
/////////////////////////////////////////////////////

//In eval.c//////////////////////////////////////////
InterpreterResult run (VM *, Frame *);
/////////////////////////////////////////////////////

//In loader.c for now////////////////////////////////
LoaderResult Instantiate(VM *, uint8_t *, size_t)
/////////////////////////////////////////////////////

#ifdef __cplusplus
    }
#endif

#endif