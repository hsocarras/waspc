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
#include "runtime/store.h"



#include <stdint.h>
#include <string.h>
#include <time.h>


static StackValue GetDefaultValue (uint8_t type){

    StackValue val;
    val.type = type;
    switch (type)
    {
    case WAS_VAL_TYPE_I32:
        val.value.i32 = 0;
        break;
    case WAS_VAL_TYPE_I64:
        val.value.i64 = 0;
        break;
    case WAS_VAL_TYPE_F32:
        val.value.f32 = 0;
        break;
    case WAS_VAL_TYPE_F64:
        val.value.f64 = 0;
        break;
    case WAS_VAL_TYPE_V128:
        for (size_t i = 0; i < 16; i++)
        {
            val.value.v128[i] = 0;
        } 
        break;    
    default:
        val.value.i64 = 0;
        break;
    }
    return val;    
}

/**
 * @brief This function traverse the function's locals on binary and pushing the correspondent default value onto the value stack 
 */
static uint32_t InitLocals (WpInterpreterState *self, const uint8_t *locals){

    const uint8_t *index  = locals;    
    uint32_t locals_count = 0;
    uint32_t type_len;
    uint32_t locals_pushed_counter= 0;
    StackValue val;

    index = DecodeLeb128Int32(index, &locals_count);
    if(!index){
        return 0;
    }

    for(uint32_t i = 0; i < locals_count; i++){
        index = DecodeLeb128Int32(index, &type_len);
        if(!index){
            return 0;
        }
        for (uint32_t ii = 0; ii < type_len; ii++)
        {
            val = GetDefaultValue(index[ii]);
            PushValue(self, val);
            index++;
            locals_pushed_counter++;

        }        
    }

    return locals_pushed_counter;

    
}

static uint8_t EvalFrame (WpInterpreterState *self, CallFrame *frame){

    uint8_t instruction;
    WpGlobalInstance *global;
    StackValue c1, c2, c3;
    uint32_t aux_u32;
    CtrlFrameType ctrl_type;
   
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
            case OPCODE_RETURN:
            _return:
            ctrl_type = self->ctrl_satck[self->ctrl_count-1].type;
            switch (ctrl_type)
            {
            case WP_INTERPRETER_CTRL_CALL_FRAME:
                self->ctrl_count--;                break;
            case WP_INTERPRETER_CTRL_LABEL:
                self->ctrl_count--;
                goto _return;
                break;
            case WP_INTERPRETER_CTRL_HANDLER:
                break;
            default:
                break;
            }  
            case OPCODE_LOCAL_GET:
                self->ip = DecodeLeb128Int32(self->ip, &aux_u32); //read local idx
                if(!self->ip){
                    return 1;
                }
                if(aux_u32 >= frame->locals_count){
                    return 3;
                }       
                c1 = frame->bp[aux_u32];  
                PushValue(self, c1);
                break;
            case OPCODE_LOCAL_SET:
                if(self->value_stack_top > self->value_stack){
                    self->ip = DecodeLeb128Int32(self->ip, &aux_u32); //read local idx
                    if(!self->ip){
                        return 1;
                    }
                    c1 = PopValue(self);
                    frame->bp[aux_u32] = c1;
                    break;
                }
                else return 5;  
            case OPCODE_GLOBAL_GET:
                self->ip = DecodeLeb128Int32(self->ip, &aux_u32); //read global idx
                if(!self->ip){
                    return 1;
                }
                global = &frame->module->globals[aux_u32];
                if(!global){
                    return 10;
                }
                memcpy(&c1, &global->val, sizeof(StackValue));
                PushValue(self, c1);
                break;
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
            case OPCODE_I32_ADD:
                c1 = PopValue(self);
                c2 = PopValue(self);
                if(c1.type != WAS_VAL_TYPE_I32 || c2.type != WAS_VAL_TYPE_I32){
                    return 3;
                }
                c3.type = WAS_VAL_TYPE_I32;
                c3.value.i32 = c2.value.i32 + c1.value.i32;
                PushValue(self, c3);
                break;
            default:
                return 4;
                break;

        }

        //updating watch dog
        //current_t = clock();
    }

    #undef READ_BYTE

}

static uint8_t InvokeFunctionRef(WpInterpreterState *self, StackValue func_ref){
    uint32_t locals_len;
    StackValue *bp;
    

    //STEP 1
    
    uint32_t address = func_ref.value.i32;
    WpFunctionInstance *func = WpStoreGetFunctionByIndex(self->store, address);
    if(!func){
        return 1;
    }
    //STEP 5 
    if(self->value_stack_top - self->value_stack < func->param_len){
        return 2;
    }
    bp = self->value_stack_top - func->param_len;
    //STEP 6
    locals_len = InitLocals(self, func->locals);
    
    //STEP 7 and 8  
    CtrlFrame * ctrl = &self->ctrl_satck[self->ctrl_count++]; //get top and add frame count
    ctrl->type = WP_INTERPRETER_CTRL_CALL_FRAME;    
    ctrl->ctrl.call_frame.bp = bp;
    ctrl->ctrl.call_frame.locals_count = func->param_len + locals_len;
    ctrl->ctrl.call_frame.arity = func->ret_len;
    ctrl->ctrl.call_frame.module = func->module;
    CallFrame *frame = &ctrl->ctrl.call_frame;

    //STEP 9    
    ctrl = &self->ctrl_satck[self->ctrl_count++];
    ctrl->type = WP_INTERPRETER_CTRL_LABEL;
    ctrl->ctrl.label =  func->ret_len;

    self->ip = func->body;
    return EvalFrame(self, frame);

}


StackValue WpInterpreterEvalExpr(WpInterpreterState *self, const uint8_t *code){
    StackValue result;
    uint8_t error_code;
    self->ip = code;
    error_code = EvalFrame(self, NULL);
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

uint8_t WpInterpreterExecuteCallRefFunc(WpInterpreterState *self, const uint8_t *type){
    StackValue result;
    StackValue ref_func;

    ref_func = PopValue(self);
    if(ref_func.type != WAS_VAL_REF_FUNC){
        return 1;
    }
    
    return 0;    
}