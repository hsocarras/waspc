/**
 * @file stack.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-02
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "vm/vm.h"
#include "webassembly/structure/types.h"

#include <string.h>

/**
 * @brief Push a value to stack.
 * The stack is a raw byte array, first byte for a value represents its type.
 * 
 * @param vm Virtual machine instance.
 * @param val Value to push
 * @return WpResult
 */
WpResult VmPushValue(VM *vm, VmValue value){

    WpResult result;      //wasp result object
    WpResultInit(&result);
    
    uint8_t *stack_end = &vm->value_stack[VM_VALUE_STACK_SIZE];
    switch (value.type)
    {
    case WAS_I32:
        //Check to prevent stack overflow
        if(stack_end - vm->value_stack_top >= 5){
            //first save data to the stack
            //copy the value into the stack
            memcpy(vm->value_stack_top, &value.as.u32, 4);
            vm->value_stack_top += 4;
            //push data type on top of the stack for easy data type check
            *vm->value_stack_top = WAS_I32;
            vm->value_stack_top++;            
            //return number of byte write type + data lenght
            result.value.u32 = 5;
            return result;
        }
        else{
            WpResultAddError(&result, WP_DIAG_ID_STACK_OVERFLOW, W_DIAG_MOD_LIST_VM);
            return result;
        }        
        break;    
    case WAS_I64:
        //Check to prevent stack overflow
        if(stack_end - vm->value_stack_top >= 9){
            //first save type into the stack
            //copy the value into the stack
            memcpy(vm->value_stack_top, &value.as.u64, 8);
            vm->value_stack_top += 8;
            //push data type on top of the stack for easy data type check
            *vm->value_stack_top = WAS_I64;
            vm->value_stack_top++;            
            //return number of byte write type + data lenght
            result.value.u32 = 9;
            return result;
        }
        else{
            WpResultAddError(&result, WP_DIAG_ID_STACK_OVERFLOW, W_DIAG_MOD_LIST_VM);
            return result;
        }        
        break; 
    case WAS_F32:
        //Check to prevent stack overflow
        if(stack_end - vm->value_stack_top >= 5){
            //first save type into the stack
            //copy the value into the stack
            memcpy(vm->value_stack_top, &value.as.f32, 4);
            vm->value_stack_top += 4;
            //push data type on top of the stack for easy data type check
            *vm->value_stack_top = WAS_F32;
            vm->value_stack_top++;            
            //return number of byte write type + data lenght
            result.value.u32 = 5;
            return result;
        }
        else{
            WpResultAddError(&result, WP_DIAG_ID_STACK_OVERFLOW, W_DIAG_MOD_LIST_VM);
            return result;
        }        
        break;  
        case WAS_F64:
        //Check to prevent stack overflow
        if(stack_end - vm->value_stack_top >= 9){
            //first save type into the stack
            //copy the value into the stack
            memcpy(vm->value_stack_top, &value.as.f64, 8);
            vm->value_stack_top += 8;
             //push data type on top of the stack for easy data type check
            *vm->value_stack_top = WAS_F64;
            vm->value_stack_top++;            
            //return number of byte write type + data lenght
            result.value.u32 = 9;
            return result;
        }
        else{
            WpResultAddError(&result, WP_DIAG_ID_STACK_OVERFLOW, W_DIAG_MOD_LIST_VM);
            return result;
        }        
        break;         
    default:
        WpResultAddError(&result, WP_DIAG_ID_STACK_ASSERT_DATA_TYPE, W_DIAG_MOD_LIST_VM);
        return result;       
    }

}

/**
 * @brief pop a value from stack
 * 
 * @param vm Virtual machine instance. 
 * @return VmValue 
 */
VmValue VmPopValue(VM *vm){

    vm->value_stack_top--;
    ValType data_type = *vm->value_stack_top;
    VmValue value;

    value.type = data_type;     //set data type

    switch (data_type)
    {
    case WAS_I32:
        vm->value_stack_top = vm->value_stack_top - 4;        
        memcpy(&value.as.u32, vm->value_stack_top, 4); //copy value from stack to value
        return value;        
    case WAS_I64:        
        vm->value_stack_top =vm->value_stack_top - 8;
        memcpy(&value.as.u64, vm->value_stack_top, 8);
        return value; 
    case WAS_F32:        
        vm->value_stack_top = vm->value_stack_top - 4;
        memcpy(&value.as.f32, vm->value_stack_top, 4);
        return value;   
    case WAS_F64:        
        vm->value_stack_top = vm->value_stack_top - 8;
        memcpy(&value.as.f64, vm->value_stack_top, 8);
        return value;
    default:
        value.type = WAS_UNDEF;
        value.as.u64 = 0;
        return value;
        break;
    }


}