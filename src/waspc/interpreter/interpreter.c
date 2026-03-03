/**
 * @file vm.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-10-30
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "interpreter/interpreter.h"

void WpInterpreterInit(WpInterpreterState *self){

    self->ip = NULL;
    self->value_stack_top = self->value_stack;
    self->value_stack_end = &self->value_stack[VM_VALUE_STACK_SIZE - 1];

    self->call_index = 0;
    
}
