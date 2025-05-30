/**
 * @file values.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef WASPC_WEBASSEMBLY_EXECUTION_RUNTIME_VALUES_H
#define WASPC_WEBASSEMBLY_EXECUTION_RUNTIME_VALUES_H

#ifdef __cplusplus
    extern "C" {
#endif

#include "webassembly/structure/types.h"

#include <stdint.h>

/**
 * @brief 
 * 
 */
typedef struct Value{
    ValType type;
    union {
        I32 s32;
        U32 u32;
        I64 s64;
        U64 u64;
        F32 f32;
        F64 f64;
        VecType v128;
        Ref ref;
    } Val;
}Value;

typedef struct VecValue{
    uint32_t length;
    Value elements;
}VecValue;


typedef struct ExternalValue {
    ExternalType type;  
    /*union address
    {
        //FuncInst *func;
        //TableInst *table;
        //MemoryInst *memory;
        //GlobalInst *global;
    };*/
      
}ExternalValue;

typedef struct  VecExternalValue {
    uint32_t lenght;
    ExternalValue elements;
}VecExtrenalValue;


/**
 * @brief 
 * 
 */
typedef struct Trap{
    uint32_t t;
}Trap;

/**
 * @brief 
 * 
 */
typedef struct Result{
    uint8_t is_trap;
    uint32_t len;
    Value *values;
    Trap t;
} Result;



#ifdef __cplusplus
    }
#endif

#endif