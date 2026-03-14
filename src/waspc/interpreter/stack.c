/**
 * @file stack.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "interpreter/interpreter.h"

#include <string.h>

/**
 * @brief Push a value to stack.
 * The stack is a raw byte array, first byte for a value represents its type.
 * 
 * @param self Virtual machine instance.
 * @param val Value to push
 * @return WpResult
 */
void PushValue(WpInterpreterState *self, StackValue val){   
    //TODO stackoverflow 
    *self->value_stack_top = val;
    self->value_stack_top++;

}

/**
 * @brief pop a value from stack
 * 
 * @param vm Virtual machine instance. 
 * @return VmValue 
 */
StackValue PopValue(WpInterpreterState *self){  
   
    self->value_stack_top--;
    return *self->value_stack_top;    

}
