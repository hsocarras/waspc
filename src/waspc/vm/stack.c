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
#include "vm/values.h"

#include <string.h>

/**
 * @brief Push a value to stack.
 * The stack is a raw byte array, first byte for a value represents its type.
 * 
 * @param vm Virtual machine instance.
 * @param val Value to push
 * @return WpResult
 */
WpResult VmPushValue(VM *vm, VmValue entry){

    WpResult result;      //wasp result object
    WpResultInit(&result);
    
    uint8_t *stack_end = &vm->value_stack[VM_VALUE_STACK_SIZE];
    switch (entry.type)
    {
    case VM_NUM_I32:
        //Check to prevent stack overflow
        if(stack_end - vm->value_stack_top >= 5){
            //first save data to the stack
            //copy the value into the stack
            memcpy(vm->value_stack_top, &entry.val.u32, 4);
            vm->value_stack_top += 4;
            //push data type on top of the stack for easy data type check
            *vm->value_stack_top = VM_NUM_I32;
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
    case VM_NUM_I64:
        //Check to prevent stack overflow
        if(stack_end - vm->value_stack_top >= 9){
            //first save type into the stack
            //copy the value into the stack
            memcpy(vm->value_stack_top, &entry.val.u64, 8);
            vm->value_stack_top += 8;
            //push data type on top of the stack for easy data type check
            *vm->value_stack_top = VM_NUM_I64;
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
    case VM_NUM_F32:
        //Check to prevent stack overflow
        if(stack_end - vm->value_stack_top >= 5){
            //first save type into the stack
            //copy the value into the stack
            memcpy(vm->value_stack_top, &entry.val.f32, 4);
            vm->value_stack_top += 4;
            //push data type on top of the stack for easy data type check
            *vm->value_stack_top = VM_NUM_F32;
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
        case VM_NUM_F64:
        //Check to prevent stack overflow
        if(stack_end - vm->value_stack_top >= 9){
            //first save type into the stack
            //copy the value into the stack
            memcpy(vm->value_stack_top, &entry.val.f64, 8);
            vm->value_stack_top += 8;
             //push data type on top of the stack for easy data type check
            *vm->value_stack_top = VM_NUM_I32;
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
    VmValueType data_type = *vm->value_stack_top;
    VmValue entry;

    entry.type = data_type;     //set data type

    switch (data_type)
    {
    case VM_NUM_I32:
        vm->value_stack_top = vm->value_stack_top - 4;        
        memcpy(&entry.val.u32, vm->value_stack_top, 4); //copy value from stack to entry
        return entry;        
    case VM_NUM_I64:        
        vm->value_stack_top =vm->value_stack_top - 8;
        memcpy(&entry.val.u64, vm->value_stack_top, 8);
        return entry; 
    case VM_NUM_F32:        
        vm->value_stack_top = vm->value_stack_top - 4;
        memcpy(&entry.val.f32, vm->value_stack_top, 4);
        return entry;   
    case VM_NUM_F64:        
        vm->value_stack_top = vm->value_stack_top - 8;
        memcpy(&entry.val.f64, vm->value_stack_top, 8);
        return entry;
    default:
        entry.type = VM_VALUE_UNDEF;
        entry.val.u64 = 0;
        return entry;
        break;
    }


}