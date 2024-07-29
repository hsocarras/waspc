/**
 * @file eval.c
 * @author Hector E. Socarras (hsocarras1987@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#include "vm/vm.h"
#include "vm/leb128.h"
#include "vm/frame.h"

#include <stdio.h>
#include <assert.h>

/**
 * @brief It's a VM Object's funtion.
 *
 * Instantiation checks that the module is valid and the provided imports match the declared types,
 * and may fail with an error otherwise. Instantiation can also result in a trap from initializing 
 * a table or memory from an active segment or from executing the start function. 
 * It is up to the embedder to define how such conditions are reported.
 * 
 * @param self. Pointer to current virtual machine 
 * @param wasm 
 * @param len 
 * @return LoaderResult 
 */
LoaderResult Instantiate (VM *self, uint8_t *wasm, size_t len){

    // 1  If module is not valid, then:
    //    Fail

}

void Push(VM *self, Value val){

    if((self->stack_top - self->value_stack) < VM_VALUE_STACK_SIZE){
        *vm->stack_top = val;
        vm->stack_top++;
    }
    else{
        //Stack overflow
        assert (0);
    }
    
}

Value Pop (VM* self){
    
    if((self->stack_top - self->value_stack) > 0){
        vm->stack_top--;
        return *vm->stack_top;
    }
    assert(0);
    return *vm->stack_top;
}

void CallFrame (VM *self, Value val){

    if(self->frame_count < VM_CALL_STACK_SIZE - 1){
        vm->call_stack[frame_count] = val;
        vm->frame_count++;
    }
    else{
        //MAx call stack 
        assert (0);
    }

}

void ReturnFromFrame(VM *self){

    if(self->frame_count > 0){
        vm->call_stack[frame_count] = null_frame;
        vm->frame_count--;
        return *vm->stack_top;
    }
    assert(0);
    return *vm->stack_top;

}
