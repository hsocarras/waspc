/**
 * @file eval.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-10-30
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "interpreter/interpreter.h"
#include "webassembly/instructions.h"
#include "utils/leb128.h"



#include <stdint.h>


static uint8_t EvalFrame (WpInterpreterState *self){

    uint8_t instruction;
    WasValue c1, c2;
    uint32_t aux_u32;

   
    //clock_t start_t, current_t;
    //uint64_t watchdog = 2*CLOCKS_PER_SEC;    
    
    //start_t = clock();
    //current_t = clock();

    #define READ_BYTE() (*self->ip++)

    while(1){ //TODO better loop condition, watch dog to break the loop
        instruction = READ_BYTE();        

        switch(instruction){
            case OPCODE_END:                
                return 0; //no error            
            case OPCODE_I32_CONST:
                c1.type = WAS_VAL_TYPE_I32;
                self->ip = DecodeLeb128Int32(self->ip, &c1.value.i32);    //5.2.2
                if(!self->ip){
                    return 1;
                }
                if(self->value_stack_top >= self->value_stack_end){
                    return 2; //stack overflow
                }
                PushValue(self, c1);
                break;
        }

        //updating watch dog
        //current_t = clock();
    }

    #undef READ_BYTE

}

WasValue WpInterpreterEvalExpr(WpInterpreterState *self, const uint8_t *code){
    WasValue result;
    uint8_t error_code;
    self->ip = code;
    error_code = EvalFrame(self);
    if(error_code != 0){
        result.type = WAS_EX_VAL_TYPE_NULL; //TODO better error handling, maybe return an error object instead of a value
        result.value.i32 = error_code; //TODO better error handling, maybe return an error object instead of a value
        return result;
    }
    if(self->value_stack_top == self->value_stack){
        result.type = WAS_EX_VAL_TYPE_NULL; //TODO better error handling, maybe return an error object instead of a value
        result.value.i32 = error_code; //TODO better error handling, maybe return an error object instead of a value
        return result;
    }
    result = PopValue(self);
    return result;
}