/**
 * @file frame.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-25
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef WASPC_INTERPRETER_CTRL_FRAME_H
#define WASPC_INTERPRETER_CTRL_FRAME_H

#ifdef __cplusplus
    extern "C" {
#endif

#include "webassembly/values.h"

// Define the types for WebAssembly values
typedef enum StackValType {
    WAS_EX_VAL_TYPE_NULL = 0x00,
    WAS_VAL_TYPE_I32 = 0x7F,
    WAS_VAL_TYPE_I64 = 0x7E,
    WAS_VAL_TYPE_F32 = 0x7D,
    WAS_VAL_TYPE_F64 = 0x7C,
    WAS_VAL_TYPE_V128 = 0x7B,
    WAS_VAL_REF_FUNC ,
} StackValType;

typedef struct SatckValue{
    StackValType type;
    union val{
        I32 i32;
        I64 i64;
        F32 f32;
        F64 f64;
        V128 v128;      //TODO make a vector stack on intterpreter and store a index on stack to reduce memory size for stackvalue to 64 bit
    } value;
} StackValue;


#ifdef __cplusplus
    }
#endif

#endif