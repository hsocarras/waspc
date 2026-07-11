/**
 * @file wasm_veval.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-21
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "validator/wasm_validator.h"
#include "decoder/wasm_decoder.h"
#include "interpreter/values.h"
#include "webassembly/instructions.h"
#include "decoder/leb128.h"
#include "decoder/ieee_754.h"

#include <stdint.h>


void ValPushValue(WpValidatorState *self, StackValue val){   
    //TODO stackoverflow 
    *self->value_stack_top = val;
    self->value_stack_top++;

}

StackValue ValPopValue(WpValidatorState *self){  
   
    self->value_stack_top--;
    return *self->value_stack_top;    

}

/**
 * @brief Function to evaluate a WebAssembly opcode in the validator.
 * This function processes the opcode and updates the validator state accordingly.  
 * It checks if the opcode is valid and if the value stack has enough space for the operation.
 * @param self Pointer to the validator state
 * @param opcode The opcode to evaluate
 * @return uint32_t 0 on error, 1 on success
 */
static uint32_t WpValidateOpcode(WpValidatorState *self, OpCode opcode) {

    StackValue val;             //auxiliary variable for decoded values  
    uint32_t err_code;
   
    
    switch (opcode) {        
        case OPCODE_I32_CONST:
            // Push I32 type onto the stack
            self->ip = DecodeLeb128Int32(self->ip, &val.value.i32);
            if(!self->ip){
                err_code = 1;
                return err_code;
            }
            val.type = WAS_VAL_TYPE_I32;
            ValPushValue(self, val); 
            return 0;           
            break;
        case OPCODE_I64_CONST:
            // Push I64 type onto the stack
            self->ip = DecodeLeb128Int64(self->ip, &val.value.i64);
            if(!self->ip){
                err_code = 2;
                return err_code;
            }
            val.type = WAS_VAL_TYPE_I64;
            ValPushValue(self, val);
            return 0;
            break;
        case OPCODE_F32_CONST:
            // Push F32 type onto the stack
            self->ip = DecodeF32(self->ip, &val.value.f32);
            if(!self->ip){
                err_code = 3;
                return err_code;
            }
            val.type = WAS_VAL_TYPE_F32;
            ValPushValue(self, val);
            return 0;
            break;
        case OPCODE_F64_CONST:
            // Push F64 type onto the stack
            self->ip = DecodeF64(self->ip, &val.value.f64);
            if(!self->ip){
                err_code = 4;
                return err_code;
            }
            val.type = WAS_VAL_TYPE_F64;
            ValPushValue(self, val);
            return 0;
            break;  
        case OPCODE_END:
            // Pop the stack, expecting an end of expression
            return 0;
        default:
            err_code = 300;
            return err_code; // Unsupported opcode
    }
}

/**
 * @brief function to validate constant expresions used to init globals
 * TODO rest of constant expresions
 * @param 
 * @param
 * @param
 * @return error code. 0 no error.
 */
uint32_t WpValidateConstantExpresion(WpValidatorState *self,StackValType type, const uint8_t *index, const uint8_t *end_index) {
    
    uint8_t opcode;
    uint32_t error_code;
    StackValue val;
    self->ip = index;
    
    #define READ_BYTE() (*self->ip++)    
    
    while (self->ip < end_index)
    {
        opcode = READ_BYTE();
        switch (opcode)
        {
        case OPCODE_I32_CONST:
        case OPCODE_I64_CONST:
        case OPCODE_F32_CONST:
        case OPCODE_F64_CONST:
            
            error_code = WpValidateOpcode(self, opcode);
            if(error_code > 0){
                return error_code;
            }
            break;        
        case OPCODE_END:
            error_code = WpValidateOpcode(self, opcode);
            if(error_code > 0){
                return error_code;
            }
            //get the eval result
            val = ValPopValue(self);
            if(val.type != type){
                error_code = 303;
                return error_code;
            }
            return 0;
        default:
            error_code = 301;
            return error_code;
        }
    }
    
    return 309; // No end opcode found
    #undef READ_BYTE
    
}