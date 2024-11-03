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

#include "vm/vm.h"

void VmInit(VM *self){

    self->byte_code = NULL;
    self->ip = NULL;

    self->value_stack_top = self->value_stack;      //reset the stack pointer
}

void VmFree(VM *self){

}


WpResult VmExecute(VM *self, const uint8_t * code){
    
    WpResult result;      //wasp result object
    WpResultInit(&result);
    self->byte_code = code;
    self->ip = code;
    
    return VmEvalFrame(self);
}