/**
 * @file vm.h
 * @author Hector E. Socarras (hsocarras1987@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-28
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef WASPC_VM_VM_H
#define WASPC_VM_VM_H

#ifdef __cplusplus
    extern "C" {
#endif

#include "object/result.h"

#include <stdint.h>

#define VM_VALUE_STACK_SIZE 512       //later must see how to include the PortConfig.h with Cmake and a target variable

#ifndef VM_VALUE_STACK_SIZE
        #error Missing definition:  VM_MAX_STACK_SIZE must be defined in PortConfig.h.
#endif

#define VM_CALL_STACK_SIZE = 24       //must not be greater than 255


/**
 * @brief Struct for Virtual machine object
 * 
 */
typedef struct VM{
    
    const uint8_t * byte_code;
    const uint8_t * ip;
    
}VM;

void VmInit(VM *);

void VmFree(VM *);

WpResult VmExecute(VM *, const uint8_t *);

WpResult VmEvalFrame (VM *);

#ifdef __cplusplus
    }
#endif

#endif