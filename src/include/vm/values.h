/**
 * @file values.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef WASPC_VM_VALUES_H
#define WASPC_VM_VALUES_H

#ifdef __cplusplus
    extern "C" {
#endif

#include "object/result.h"

#include <stdint.h>

typedef enum VmValueType{
    VM_VALUE_UNDEF,
    VM_NUM_I32,
    VM_NUM_I64,
    VM_NUM_F32,
    VM_NUM_F64,
}VmValueType;

typedef struct VmValue
{
    VmValueType type;
    union
    {
        uint32_t u32;
        int32_t s32;
        uint64_t u64;
        int64_t s64;
        float f32;
        double f64;
    }val;
    
}VmValue;




#ifdef __cplusplus
    }
#endif

#endif