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
#include "webassembly/bin.h"
#include "utils/leb128.h"
#include "utils/ieee_754.h"
#include "decoder/wasm_decoder.h"

#include <stdint.h>
#include <string.h>
#include <time.h>
#include <math.h>
//#include <stdio.h>

/**
 * @brief This function returns the default value for a given WebAssembly value type.
 * @param type The WebAssembly value type (e.g., WAS_VAL_TYPE_I32, WAS_VAL_TYPE_I64, etc.).
 * @return StackValue The default value for the specified type, initialized to zero or equivalent.
 */
static StackValue GetDefaultValue(uint8_t type)
{

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
static uint32_t InitLocals(WpInterpreterState *self, const uint8_t *locals)
{

    const uint8_t *index = locals;
    uint32_t locals_count = 0;
    uint32_t type_len;
    uint32_t locals_pushed_counter = 0;
    StackValue val;

    index = DecodeLeb128Int32(index, &locals_count);
    if (!index)
    {
        return 0;
    }

    for (uint32_t i = 0; i < locals_count; i++)
    {
        index = DecodeLeb128Int32(index, &type_len);
        if (!index)
        {
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

/**
 * @brief This function is responsible for invoking a function by its index.
 * It sets up the call frame, initializes local variables, and manages the control flow for the function execution.
 * It returns an error code if any step of the invocation process fails.
 * Implement the steps 1 to 10 of the function invocation as described in the WebAssembly specification 4.6.5.
 * @param self Pointer to the interpreter state.
 * @param func_index Index of the function to invoke.
 * @return uint32_t Error code. 0 on success, non-zero on failure.
 */
static uint32_t InvokeFunction(WpInterpreterState *self, uint32_t func_index)
{

    // STEP 1 Assert: due to validation, 𝑆.funcs[𝑎] exists
    if (!self->store->funcs || func_index >= self->store->func_count)
    {
        return 1; // TODO better error handling, maybe return an error object instead of an error code TRAP
    }
    // STEP 2 Let 𝑓 be the function instance, 𝑆.funcs[𝑎].
    WpFunctionInstance *func = WpStoreGetFunctionByIndex(self->store, func_index);
    if (!func)
    {
        return 2;
    }

    if (func->wp_type == WP_OBJECT_FUNCTION_INSTANCE)
    {
        return InvokeFunctionFast(self, (WpFunctionInstance *)func);
    }

    return 3; // TODO better error handling, maybe return an error object instead of an error code TRAP
}

/**
 * @brief This function is responsible for executing a function call frame.
 * It fetches and executes instructions until it reaches the end of the frame or encounters a return instruction.
 * It manages the control flow and stack operations for the function execution.
 */
static uint32_t EvalFrame(WpInterpreterState *self, CallFrame *frame)
{

    uint8_t instruction;    
    StackValue c1, c2, c3;
    StackValue *peeked_val;
    uint32_t aux_u32;
    uint32_t error_code;
    CallFrame *current_frame = frame; // a frae parameter will be used for recursion
    WpGlobalInstance *global;
    WasmBinMemArg mem_args;
    WpMemoryInstance *memory;
    WpFunctionInstance *called_func;
    // uint64_t watchdog = 2*CLOCKS_PER_SEC;

    // start_t = clock();
    // current_t = clock();

#define READ_BYTE() (*current_frame->ip++)
#define instruction_pointer current_frame->ip

    while (1)
    { // TODO better loop condition, watch dog to break the loop and opcode end
        instruction = READ_BYTE();
        //printf("Executing instruction: 0x%02X\n", instruction);
        switch (instruction)
        {
        case OPCODE_END:
        {
            // There is no label so execute Case 1 Frame
            // PUSH retunr values where the current frame start on the stack.
            for (uint32_t i = 0; i < current_frame->arity; i++)
            {
                memcpy(current_frame->locals + i, self->value_stack_top - (current_frame->arity - i), sizeof(StackValue));
            }
            // after for loop, the sactk pointer should be were locals start + arity, which is the new top of the stack
            self->value_stack_top = current_frame->locals + current_frame->arity; // pop return values and set new top of the stack
            // check if the current frame is the initial frame of the function, if so return to caller, else continue executing the caller frame

            if (current_frame == frame)
            {
                return 0;
            }
            else
            {
                // pop the current frame and set the instruction pointer to the caller frame's ip
                // which should be right after the call instruction
                current_frame = &self->call_stack[--self->call_stack_count]; // pop current frame and get caller frame
                break;
            }
        }
        case OPCODE_RETURN:
        {
            // if(current_frame->blocks == NULL){
            // There is no label so execute Case 1 Frame
            // PUSH retunr values where the current frame start on the stack, which is the base for the function's locals (params + locals)
            for (uint32_t i = current_frame->arity; i > 0; i--)
            {
                memcpy(current_frame->locals + i, self->value_stack_top - i, sizeof(StackValue));
            }
            // after for loop, the sactk pointer should be were locals start + arity, which is the new top of the stack
            self->value_stack_top = current_frame->locals + current_frame->arity; // pop return values and set new top of the stack
            // check if the current frame is the initial frame of the function, if so return to caller, else continue executing the caller frame
            if (current_frame == frame)
            {
                return 0;
            }
            else
            {
                // pop the current frame and set the instruction pointer to the caller frame's ip
                // which should be right after the call instruction
                current_frame = &self->call_stack[--self->call_stack_count]; // pop current frame and get caller frame
                break;
            }
            //}
            /*
            else{
                //read the continuation address of the label.
                instruction_pointer = current_frame->block_top; //set instruction pointer to the continuation address of the label}
                current_frame->block_top = NULL; //reset block top
                current_frame->block_top--;
                goto _return; //execute the return instruction of the label frame
            }*/
            break;
        }
        case OPCODE_CALL:
        {              
            instruction_pointer = DecodeLeb128UInt32(instruction_pointer, &aux_u32); // read function idx
            if (!instruction_pointer)
            {
                return 1;
            }
            //
            if (aux_u32 >= frame->module->function_count)
            {
                return __LINE__; // TODO better error handling, maybe return an error object instead of an error code TRAP
            }
            
            called_func = &frame->module->funcs[aux_u32]; // get store object for functions            
            // check if the store object is a function instance or a import instance, if import instance get the function instance from the import instance
            if (called_func->wp_type !=  WP_OBJECT_FUNCTION_INSTANCE)
            {
                return __LINE__; // TODO better error handling, maybe return an error object instead of an error code TRAP
            }   
            if(called_func->func_kind == WP_FUNC_HOST)
            {
                return __LINE__; // TODO better error handling, maybe return an error object instead of an error code TRAP
            }
            if(called_func->func_kind == WP_FUNC_IMPORT)
            {
                called_func = called_func->address; // get the function instance from the import instance                
            }
            
            // STEP 3 and 4. Invoke the function instance 𝑓.
            if (InvokeFunctionFast(self, called_func) != 0)
            {
                return __LINE__; // TODO better error handling, maybe return an error object instead of an error code TRAP
            }
            // after invoking the function, the instruction pointer is set to the function body,
            // and a new frame is on the call stack  with a label in the block stack, so we can just break and continue executing the new frame
            break;
        }
        case OPCODE_CALL_REF:
        {
            // STEP 1 Assert: due to validation, a null or function reference is on the top of the stack.
            peeked_val = self->value_stack_top - 1;
            if (peeked_val->type != WAS_VAL_REF_FUNC && peeked_val->type != WAS_VAL_REF_NULL_FUNC)
            {
                return 1;
            }
            // STEP 2 Pop the reference value 𝑟 from the stack.
            c1 = PopValue(self);
            // STEP 3 If 𝑟 is a null reference, trap.
            if (c1.type == WAS_VAL_REF_NULL_FUNC)
            {
                return 2; // TODO better error handling, maybe return an error object instead of an error code TRAP
            }
            // STEP 4 Assert: due to validation, 𝑟 is a function reference.
            if (c1.type != WAS_VAL_REF_FUNC)
            {
                return 3;
            }
            // STEP 5 and 6 .Let 𝑎 be the index of the function instance referred to by 𝑟. Invoke
            if (InvokeFunction(self, c1.value.i32) != 0)
            {
                return 1; // TODO better error handling, maybe return an error object instead of an error code TRAP
            }
            // after invoking the function, the instruction pointer is set to the function body,
            // and a new frame is on the call stack  with a label in the block stack, so we can just break and continue executing the new frame
            break;
        }
        case OPCODE_LOCAL_GET:
        {
            instruction_pointer = DecodeLeb128Int32(instruction_pointer, &aux_u32); // read local idx
            if (!instruction_pointer)
            {
                return 1; // TODO better error handling, maybe return an error object instead of an error code TRAP
            }
            if (aux_u32 >= frame->locals_count)
            {
                return 3; // TODO better error handling, maybe return an error object instead of an error code TRAP
            }
            c1 = frame->locals[aux_u32];
            PushValue(self, c1);
            break;
        }
        case OPCODE_LOCAL_SET:
        {
            if (self->value_stack_top > self->value_stack)
            {
                instruction_pointer = DecodeLeb128Int32(instruction_pointer, &aux_u32); // read local idx
                if (!instruction_pointer)
                {
                    return 1; // TODO better error handling, maybe return an error object instead of an error code TRAP
                }
                c1 = PopValue(self);
                frame->locals[aux_u32] = c1;
                break;
            }
            else
                return 5; // TODO better error handling, maybe return an error object instead of an error code TRAP
        }
        case OPCODE_GLOBAL_GET:
        {   
            instruction_pointer = DecodeLeb128Int32(instruction_pointer, &aux_u32); // read global idx
            if (!instruction_pointer)
            {
                return 1; // TODO better error handling, maybe return an error object instead of an error code TRAP
            }
            
            global = &frame->module->globals[aux_u32]; // get store object for globals
            // check if the store object is a global instance or a import instance, if import instance get the global instance from the import instance
            if (global->wp_type != WP_OBJECT_GLOBAL_INSTANCE)            
            {   
                return 3;
            }
            c1 = WpGlobalInstanceGetValue(global);
            PushValue(self, c1);
            break;
        }
        case OPCODE_F64_LOAD://load a F64 from memory, to the stack.
        {
            // Step 2. Assert: Due to validation, a number value is on the top of the stack.
            if (self->value_stack_top - self->value_stack < 1)
            {
                return 5; // TODO better error handling, maybe return an error object instead of an error code TRAP
            }
            // Step 3. Pop the value (at.const 𝑖) from the stack.
            c1 = PopValue(self); // address            
            if (c1.type != WAS_VAL_TYPE_I32 && c1.type != WAS_VAL_TYPE_I64)
            {
                return 6; // TODO better error handling, maybe return an error object instead of an error code TRAP
            }

            // Read memory argument from instruction stream
            mem_args = DestructureMemArg(instruction_pointer);
            instruction_pointer = SkipMemArgBuf(instruction_pointer);

            //Step 4a
            // GET MEMORY instance, check if the memory instance exist and if the address is in the memory bounds, then store the value in memory
            if (mem_args.x >= current_frame->module->mem_count)
            {
                return 10; // TODO better error handling, maybe return an error object instead of an error code TRAP
            }
            memory = &frame->module->mems[mem_args.x];
            // check bounds
            if (c1.type == WAS_VAL_TYPE_I32)
            {
                // if(c1.value.i32 + mem_args.m + 8 > memory->usage){TODO
                // return 11;          //TODO better error handling, maybe return an error object instead of an error code TRAP
                //}
                memcpy(&c2.value.f64, memory->bytes + c1.value.i32 + mem_args.m, 8);
            }
            else
            {
                if (c1.value.i64 + mem_args.m + 8 > memory->usage)
                {
                    return 12; // TODO better error handling, maybe return an error object instead of an error code TRAP
                }
                memcpy(&c2.value.f64, memory->bytes + c1.value.i64 + mem_args.m, 8);
            }
            c2.type = WAS_VAL_TYPE_F64;
            PushValue(self, c2);
            break;
        }
        case OPCODE_F64_STORE:
        {
            // Step 2 y  4. At least 2 values on the stack, the value to store and the address
            if (self->value_stack_top - self->value_stack < 2)
            {
                return 5; // stack underflow
            }
            // Step 3 y 5. Pop the value to store and the address from the stack, in that order.
            c1 = PopValue(self); // value to store
            c2 = PopValue(self); // address
            // Step 6. If the value is not of the expected type, trap.
            if (c1.type != WAS_VAL_TYPE_F64)
            {
                return 6; // TODO better error handling, maybe return an error object instead of an error code TRAP
            }
            mem_args = DestructureMemArg(instruction_pointer);
            instruction_pointer = SkipMemArgBuf(instruction_pointer);
            if (!instruction_pointer)
            {
                return 1; // TODO better error handling, maybe return an error object instead of an error code TRAP
            }
            // GET MEMORY instance, check if the memory instance exist and if the address is in the memory bounds, then store the value in memory
            if (mem_args.x >= current_frame->module->mem_count)
            {
                return 11; // TODO better error handling, maybe return an error object instead of an error code TRAP
            }
            memory = &current_frame->module->mems[mem_args.x];
            error_code = WpMemoryStoreF64(memory, c2.value.i32 + mem_args.m, c1.value.f64);
            if (error_code != 0)
            {
                return 12; // TODO better error handling, maybe return an error object instead of an error code TRAP
            }
            break;
        }
        case OPCODE_I32_CONST:
        {
            c1.type = WAS_VAL_TYPE_I32;
            instruction_pointer = DecodeLeb128Int32(instruction_pointer, &c1.value.i32); // 5.2.2
            if (!instruction_pointer)
            {
                return 101;
            }
            if (self->value_stack_top >= self->value_stack_end)
            {
                return 2; // stack overflow
            }
            PushValue(self, c1);
            break;
        }
        case OPCODE_F64_CONST:
        {
            c1.type = WAS_VAL_TYPE_F64;
            instruction_pointer = DecodeF64(instruction_pointer, &c1.value.f64); // 5.2.2
            if (!instruction_pointer)
            {
                return 102;
            }
            if (self->value_stack_top >= self->value_stack_end)
            {
                return 2; // stack overflow
            }
            PushValue(self, c1);
            break;
        }
        case OPCODE_I32_ADD:
        {
            c1 = PopValue(self);
            c2 = PopValue(self);
            if (c1.type != WAS_VAL_TYPE_I32 || c2.type != WAS_VAL_TYPE_I32)
            {
                return 3;
            }
            c3.type = WAS_VAL_TYPE_I32;
            c3.value.i32 = c2.value.i32 + c1.value.i32;
            PushValue(self, c3);
            break;
        }
        case OPCODE_I32_SUB:
        {
            c1 = PopValue(self);
            c2 = PopValue(self);
            if (c1.type != WAS_VAL_TYPE_I32 || c2.type != WAS_VAL_TYPE_I32)
            {
                return 3;
            }
            c3.type = WAS_VAL_TYPE_I32;
            c3.value.i32 = c2.value.i32 - c1.value.i32;
            PushValue(self, c3);
            break;
        }
        case OPCODE_I32_MUL:
        {
            c1 = PopValue(self);
            c2 = PopValue(self);
            if (c1.type != WAS_VAL_TYPE_I32 || c2.type != WAS_VAL_TYPE_I32)
            {
                return 3;
            }
            c3.type = WAS_VAL_TYPE_I32;
            c3.value.i32 = c2.value.i32 * c1.value.i32;
            PushValue(self, c3);
            break;
        }
        case OPCODE_F64_SQRT:
        {
            c1 = PopValue(self);
            if (c1.type != WAS_VAL_TYPE_F64)
            {
                return 3;
            }
            c2.type = WAS_VAL_TYPE_F64;
            c2.value.f64 = sqrt(c1.value.f64);
            PushValue(self, c2);
            break;
        }
        case OPCODE_F64_ADD:
        {
            c1 = PopValue(self);
            c2 = PopValue(self);
            if (c1.type != WAS_VAL_TYPE_F64 || c2.type != WAS_VAL_TYPE_F64)
            {
                return __LINE__;
            }
            c3.type = WAS_VAL_TYPE_F64;
            c3.value.f64 = c2.value.f64 + c1.value.f64;
            PushValue(self, c3);
            break;
        }
        case OPCODE_F64_SUB:
        {
            c1 = PopValue(self);
            c2 = PopValue(self);
            if (c1.type != WAS_VAL_TYPE_F64 || c2.type != WAS_VAL_TYPE_F64)
            {
                return __LINE__;
            }
            c3.type = WAS_VAL_TYPE_F64;
            c3.value.f64 = c2.value.f64 - c1.value.f64;
            PushValue(self, c3);
            break;
        }
        case OPCODE_F64_MUL:
        {
            c1 = PopValue(self);
            c2 = PopValue(self);
            if (c1.type != WAS_VAL_TYPE_F64 || c2.type != WAS_VAL_TYPE_F64)
            {
                return __LINE__;
            }
            c3.type = WAS_VAL_TYPE_F64;
            c3.value.f64 = c2.value.f64 * c1.value.f64;
            PushValue(self, c3);
            break;
        }
        default: 
            return __LINE__; // TODO better error handling, maybe return an error object instead of an error code TRAP
            break;       

        // updating watch dog
        // current_t = clock();
        }
    }

#undef READ_BYTE
}

StackValue WpInterpreterEvalExpr(WpInterpreterState *self, CallFrame *frame)
{
    StackValue result;
    uint32_t error_code;
    error_code = EvalFrame(self, frame);
    if (error_code != 0)
    {
        result.type = WAS_EX_VAL_TYPE_NULL; // TODO better error handling, maybe return an error object instead of a value
        result.value.i32 = error_code;      // TODO better error handling, maybe return an error object instead of a value
        return result;
    }
    if (self->value_stack_top == self->value_stack)
    {
        result.type = WAS_EX_VAL_TYPE_NULL; // TODO better error handling, maybe return an error object instead of a value
        result.value.i32 = error_code;      // TODO better error handling, maybe return an error object instead of a value
        return result;
    }
    result = PopValue(self);
    return result;
}

/**
 * @brief This function is responsible for invoking a function instance directly.
 * It sets up the call frame, initializes local variables, and manages the control flow for the function execution.
 * It returns an error code if any step of the invocation process fails.
 * @addtogroup interpreter
 * @{
 *
 */
uint32_t InvokeFunctionFast(WpInterpreterState *self, WpFunctionInstance *func)
{
    //InvokeFunctionFast parameters validation
    if (!func)
    {
        return __LINE__; // TODO better error handling, maybe return an error object instead of an error code TRAP
    }
    if (func->wp_type != WP_OBJECT_FUNCTION_INSTANCE)
    {
        return __LINE__; // TODO better error handling, maybe return an error object instead of an error code TRAP
    }

    uint32_t error_code = 0;
    uint32_t locals_len;
    uint32_t param_len = func->func_type->def.func_type.param_len;
    uint32_t ret_len = func->func_type->def.func_type.ret_len;
    StackValue *bp; // declare base pointer for the frame, which is the base for the function's locals (params + locals) on the value stack

    // STEP 5 Assert: due to validation, 𝑛 values are on the top of the stack.
    if (self->value_stack_top - self->value_stack < param_len)
    {
        return 3;
    }
    // Init frame base pointer for locals. Point to the first parameter on the stack,
    // which is the base for the function's locals (params + locals)
    bp = self->value_stack_top - param_len;

    // STEP 6
    locals_len = InitLocals(self, func->locals);
    // STEP 7 and 8
    CallFrame *activation = &self->call_stack[self->call_stack_count++]; // get top and add frame count
    activation->locals = bp;
    activation->locals_count = param_len + locals_len;
    activation->arity = ret_len;
    activation->module = func->module;
    // activation->blocks = self->block_stack_top;                         //TODO labels and handlesrs support
    // activation->block_top = self->block_stack_top;                      //TODO block stack ovreflow check

    // STEP 9 Let 𝐿 be the label whose arity is 𝑚 and whose continuation is the end of the function.
    // activation->block_top = self->ip;  //return address first byte after function call

    // STEP 10 Enter the instruction sequence instr* with label 𝐿 and no values.
    activation->ip = func->body;
    error_code = EvalFrame(self, activation); 
    return error_code;
}