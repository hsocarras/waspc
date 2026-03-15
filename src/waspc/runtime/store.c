/**
 * @file allocator.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-02-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "runtime/store.h"
#include "objects/wp_objects.h"
#include "interpreter/values.h"

#include <stdlib.h>
#include <string.h>

/**
 * @brief Initializes the store with a given memory size.
 */
void WpStoreInit(WpStore *self)
{
    self->mem = NULL;
    self->mem_size = 0;
    self->mem_free = NULL; // Initially, the entire memory is free
}

WpGlobalInstance * WpStoreGetLastGlobal(WpStore *self)
{
    if (!self->globals)
    {
        return NULL;
    }

    WpGlobalInstance *current = self->globals;
    for (size_t i = 0; i < self->global_count; i++)
    {
        current = current->next;
    }
    if (!current->next)
    {
        return current;
    }
    else    return NULL;
}

WpGlobalInstance * WpStoreGetGlobalByIndex(WpStore *self, uint32_t index)
{
    if (!self->globals)
    {
        return NULL;
    }

    WpGlobalInstance *current = self->globals;
    for (size_t i = 0; i < index; i++)
    {
        current = current->next;
    }
    return current;
}

WpFunctionInstance * WpStoreGetLastFunction(WpStore *self)
{
    if(!self->funcs)
    {
        return NULL;
    }

    WpFunctionInstance *current = self->funcs;

    for (size_t i = 0; i < self->func_count; i++)       
    {
        current = current->next;
    }
    if (!current->next)
    {
        return current;
    }
    else    return NULL;

}  

WpFunctionInstance * WpStoreGetFunctionByIndex(WpStore *self, uint32_t index)
{
    if(!self->funcs)
    {
        return NULL;
    }

    WpFunctionInstance *current = self->funcs;

    for (size_t i = 0; i < index; i++)       
    {
        current = current->next;
    }
    return current;
}

WpGlobalInstance * WpStoreAllocGlobal(WpStore *self, uint8_t mut, StackValType type, StackValue val)
{
    WpGlobalInstance new_global;
    WpGlobalInstance *last_global;  
    WpGlobalInstance * address; 
    
    //Check if there is enough free memory to allocate the global
    if (self->mem_free + sizeof(WpGlobalInstance) > self->mem + self->mem_size)
    {
        return NULL; // Not enough memory
    }

    // Init new global
    WpGlobalInstanceInit(&new_global);    
    new_global.mut = mut;
    new_global.type = type;
    new_global.val = val;

    // alloc new global on store
    memcpy(self->mem_free, &new_global, sizeof(WpGlobalInstance)); // Allocate memory for global    

    if(self->global_count > 0) { //Not first global
        last_global = WpStoreGetLastGlobal(self);
        if(!last_global){
            return NULL;        
        }
        last_global->next = (WpGlobalInstance *)self->mem_free;  // update linked list
        
    }
    else{
        self->globals = (WpGlobalInstance *)self->mem_free;
    }


    self->global_count++;
    address = (WpGlobalInstance *)self->mem_free;
    self->mem_free += sizeof(WpGlobalInstance); // Move the free pointer  
    return address; // Return the address of the allocated global
}
   
WpFunctionInstance * WpStoreAllocFunction(WpStore *self, WpModuleState *mod, WasmBinFuncType func_type, const uint8_t *locals, const uint8_t *body)
{
    WpFunctionInstance new_func;
    WpFunctionInstance *last_func;  
    WpFunctionInstance * address; 

     //Check if there is enough free memory to allocate the global
    if (self->mem_free + sizeof(WpFunctionInstance) > self->mem + self->mem_size)
    {
        return NULL; // Not enough memory
    }

    // Init new function
    WpFunctionInstanceInit(&new_func);
    new_func.module = mod;
    new_func.param_len = func_type.param_len;
    new_func.param_types = func_type.param_types;
    new_func.ret_len = func_type.ret_len;
    new_func.ret_types = func_type.ret_types;
    new_func.locals = locals;
    new_func.body = body;

    //alloc function
    memcpy(self->mem_free, &new_func, sizeof(WpFunctionInstance));  // Allocate memory for function

    if(self->func_count > 0) { //Not first function
        last_func = WpStoreGetLastFunction(self);
        if(!last_func){
            return NULL;        
        }
        last_func->next = (WpFunctionInstance *)self->mem_free;  // update linked list
        
    }
    else{
        self->funcs = (WpFunctionInstance *)self->mem_free;
    }

    self->func_count++;
    address = (WpFunctionInstance *)self->mem_free;
    self->mem_free += sizeof(WpFunctionInstance);               // Move the free pointer
    return address;         // Return the address of the allocated global
}

