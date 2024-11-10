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
#include "runtime/store.h"
#include "webassembly/execution/runtime/values.h"
#include "webassembly/execution/runtime/instances.h"

#include <stdint.h>

#define VM_VALUE_STACK_SIZE 4096      //later must see how to include the PortConfig.h with Cmake and a target variable

#ifndef VM_VALUE_STACK_SIZE
        #error Missing definition:  VM_MAX_STACK_SIZE must be defined in PortConfig.h.
#endif

#define VM_CALL_STACK_SIZE = 24       //must not be greater than 255

/**
 * @brief 
 * 
 */
typedef struct VmValue
{
    ValType type;       
    Val as;    
}VmValue;

/**
 * @brief Struct for Virtual machine object
 * 
 */
typedef struct VM{
    
    const uint8_t * byte_code;
    const uint8_t * ip;

    uint8_t value_stack[VM_VALUE_STACK_SIZE];       //value stack implemented with array of byte instead value struct for memory optimization    
    uint8_t * value_stack_top;

    Store *store;
    ModuleInst mod;
    
}VM;

/// @brief constructor for vm struct
/// @param  pointer to vm.
void VmInit(VM *);

void VmFree(VM *);

Val VmExecuteExpresion(VM *self, const uint8_t * exp);

/// @brief wraper to call VmEvalFrame function. Perform some nit steps.
/// @param  pointer to vm instance.
/// @param  code to execute.
/// @return 
WpResult VmExecuteFrame(VM *, const uint8_t *);

/// @brief Main evaluaction loop
/// @param  
/// @return 
WpResult VmEvalFrame (VM *);

/// @brief function to push a value into value stack
/// @param  vm Virtual machine instance.
/// @param  val Value to push.
/// @return bytes pushed into stack.
WpResult VmPushValue(VM *vm, VmValue val);

/// @brief function to pop a value from stack
/// @param  vm Virtual machine instance.
/// @param  val_type Type value to push
/// @return 
VmValue VmPopValue(VM *vm);

WpResult InstantiateModule(VM *self, const uint8_t * const buf, const uint32_t size);

#ifdef __cplusplus
    }
#endif

#endif