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

#include "vm/vm.h"
#include "webassembly/structure/types.h"

#include <string.h>

/**
 * @brief Push a value to stack.
 * The stack is a raw byte array, first byte for a value represents its type.
 * 
 * @param self Virtual machine instance.
 * @param val Value to push
 * @return WpResult
 */
WpResult VmPushValue(VM *self, VmValue value){

    WpResult result;      //wasp result object
    WpResultInit(&result);
    
    //Check for stack overflow    
    if(self->value_stack_top > self->value_stack_end){
        WpResultAddError(&result, WP_DIAG_ID_STACK_OVERFLOW, W_DIAG_MOD_LIST_VM);
        return result;
    }

    *self->value_stack_top = value;
    self->value_stack_top++;

}

/**
 * @brief pop a value from stack
 * 
 * @param vm Virtual machine instance. 
 * @return VmValue 
 */
WpResult VmPopValue(VM *self){

    WpResult result;                //wasp result object
    WpResultInit(&result);

    //Check for stack overflow    
    if(self->value_stack_top < self->value_stack){
        WpResultAddError(&result, WP_DIAG_ID_STACK_OVERFLOW, W_DIAG_MOD_LIST_VM);
        return result;
    }

    self->value_stack_top--;
    result.value.was = *self->value_stack_top;
    

}