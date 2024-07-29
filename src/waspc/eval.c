/**
 * @file eval.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#include "vm/vm.h"
#include "vm/leb128.h"

#include <stdio.h>
#include <assert.h>

/**
 * @brief Main interpreter loop
 * This funcion implemente the bytecode dispacher and execution loop.
 * @param self Pointer to a vm object.
 * @return InterpreterResult 
 */
static InterpreterResult eval(VM * self) {

    DEC_INT32_LEB128 dec_i32;
    DEC_UINT32_LEB128 dec_u32;
    Value stack_item1, stack_item2;     // temp values to store stack operations

    Frame *exec_frame = &self->call_stack[frame_count-1];
    self->ip = exec_frame->ip;

    #define READ_BYTE() (*self->ip++)

    for (size_t i = 0; i < exec_frame->code_len; i++) {

        uint8_t instruction;

        switch (instruction = READ_BYTE()) {
            
            case OP_RETURN: {
                return INTERPRET_OK;
            }
            case OP_I32_CONST: {
                //push constant value on top of stack
                dec_i32 = DecodeLeb128Int32(vm->ip);    // decoding i32 literal
                vm->ip = vm->ip + dec_i32.len;          // incrementing instruction pointer to next instruction
                stack_item1.type = I32;
                stack_item1.as.i32 = dec_i32.value;         // getting the value
                push(vm, stack_item1);                   // pushing into the stack
            }
            case OP_I32_SUB:{
                //
            }
            default:{

            }
        }

        i++; //avoid infinite loop
    }

    #undef READ_BYTE
    return INTERPRET_OK;
}

/**
 * @brief Wrapper function for eval
 * 
 * @param self 
 * @param pou 
 * @return InterpreterResult 
 */
InterpreterResult run(VM *self, Frame *pou){

    vm->block = pou;
    vm->ip = pou->code;
    return run(vm);

}