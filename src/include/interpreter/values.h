/**
 * @file values.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-25
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef WASPC_VM_VALUES_H
#define WASPC_VM_VALUES_H

#ifdef __cplusplus
    extern "C" {
#endif

#include "webassembly/execution/runtime/values.h"

#include <stdint.h>

/**
 * @brief 
 * 
 */
typedef struct VmValue
{
    ValType type;       
    Val as;    
}VmValue;

#ifdef __cplusplus
    }
#endif

#endif