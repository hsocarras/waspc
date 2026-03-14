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

    self->value_stack_top = NULL;
    self->value_stack = NULL;
    self->value_stack_end = NULL;

    self->ctrl_count = 0;

    self->store = NULL;
    
}
