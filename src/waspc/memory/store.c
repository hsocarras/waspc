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
#include "memory/store.h"
#include "interpreter/values.h"

#include <stdlib.h>
#include <string.h>

/**
 * @brief Initializes the store with a given memory size.
 */
void WpStoreInit(WpStore *self)
{
    self->buffer = NULL;
    self->buffer_size = 0;
    self->buffer_free = NULL; // Initially, the entire memory is free

    self->globals = NULL;
    self->global_count = 0;

    self->funcs = NULL;
    self->func_count = 0;

    self->exports = NULL;
    self->export_count = 0;

    self->memories = NULL;
    self->memory_count = 0;
}

/////GET FUNCTIONS/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
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

WpMemoryInstance * WpStoreGetMemoryByIndex(WpStore *self, uint32_t index)
{
    if(!self->memories)
    {
        return NULL;
    }

    WpMemoryInstance *current = self->memories;

    for (size_t i = 0; i < index; i++)       
    {
        current = current->next;
    }
    return current;

}

WpExportInstance * WpStoreGetExportByIndex(WpStore *self, uint32_t index)
{
    if(!self->exports)
    {
        return NULL;
    }

    WpExportInstance *current = self->exports;

    for (size_t i = 0; i < index; i++)       
    {
        current = current->next;
    }
    return current;
}

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
///// ALLOC FUNCTIONS ////////////////////////////////////////////////////////////////////////////////////////////////////////////
WpGlobalInstance * WpStoreAllocGlobal(WpStore *self, uint8_t mut, StackValType type, StackValue val)
{
    WpGlobalInstance new_global;
    WpGlobalInstance *last_global;  
    WpGlobalInstance * address; 
    
    //Check if there is enough free memory to allocate the global
    if (self->buffer_free + sizeof(WpGlobalInstance) > self->buffer + self->buffer_size)
    {
        return NULL; // Not enough memory
    }
   
    // Init new global
    WpGlobalInstanceInit(&new_global);    
    new_global.mut = mut;
    new_global.type = type;
    new_global.val = val;
    // alloc new global on store
    memcpy(self->buffer_free, &new_global, sizeof(WpGlobalInstance)); // Allocate memory for global    
    
    if(self->global_count > 0) { //Not first global
        last_global = WpStoreGetGlobalByIndex(self, self->global_count - 1); //get last global
        if(!last_global){
            return NULL;        
        }
        last_global->next = (WpGlobalInstance *)self->buffer_free;  // update linked list
        
    }
    else{
        self->globals = (WpGlobalInstance *)self->buffer_free;
    }


    self->global_count++;
    address = (WpGlobalInstance *)self->buffer_free;
    self->buffer_free += sizeof(WpGlobalInstance); // Move the free pointer  
    return address; // Return the address of the allocated global
}

WpMemoryInstance * WpStoreAllocMemory(WpStore *self, WasmBinMemory mem, uint8_t *data_memory)
{
    WpMemoryInstance new_mem;
    WpMemoryInstance *last_mem;  
    WpMemoryInstance * address;

    //Check if there is enough free memory to allocate the memory
    if (self->buffer_free + sizeof(WpMemoryInstance) > self->buffer + self->buffer_size)
    {
        return NULL; // Not enough memory
    }

    // Init new memory
    WpMemoryInstanceInit(&new_mem);
    new_mem.addres_type = mem.address_type;
    new_mem.page_size_min = mem.page_size_min;
    new_mem.page_size_max = mem.page_size_max;
    new_mem.bytes = data_memory;                    //asign data memory to memory instance, the data memory.

    //alloc memory
    memcpy(self->buffer_free, &new_mem, sizeof(WpMemoryInstance));  // Allocate memory for memory

    if(self->memory_count > 0) { //Not first memory    
        last_mem = WpStoreGetMemoryByIndex(self, self->memory_count - 1);
        if(!last_mem){
            return NULL;        
        }
        last_mem->next = (WpMemoryInstance *)self->buffer_free;  // update linked list
        
    }
    else{
        self->memories = (WpMemoryInstance *)self->buffer_free;
    }

    self->memory_count++;
    address = (WpMemoryInstance *)self->buffer_free;
    self->buffer_free += sizeof(WpMemoryInstance);               // Move the free pointer
    return address;         // Return the address of the allocated global

}

WpExportInstance * WpStoreAllocExport(WpStore *self, WasmBinExport exp, WpModuleState *mod)
{
    WpExportInstance new_export;
    WpExportInstance *last_export;  
    WpExportInstance * address;

    //check if there is enought free memory to allocate
    if (self->buffer_free + sizeof(WpExportInstance) > self->buffer + self->buffer_size)
    {
        return NULL; // Not enough memory
    }
    // Init new export
    WpExportInstanceInit(&new_export);
    new_export.name_len = exp.name_len;
    new_export.name = exp.name;
    new_export.export_type = exp.index_type;
    //Get address from index and assign to export instance
    switch (exp.index_type)
    {
        case 0x00: //func
            if(mod->funcs == NULL){
                return NULL; // No functions allocated in the store
            }
            new_export.address = (uint8_t *)&mod->funcs[exp.external_index];
            break;
        case 0x01: //Table
            //TODO
            break;
        case 0x02: //memory
            //TODO
            break;
        case 0x03: //global
                //TODO
                break;
        case 0x04: //tag
                //TODO
                break;
        default:
            return NULL;

    }

    //alloc export
    memcpy(self->buffer_free, &new_export, sizeof(WpExportInstance));  // Allocate memory for export

    if(self->export_count > 0) { //Not first export    
        last_export = WpStoreGetExportByIndex(self, self->export_count - 1);
        if(!last_export){
            return NULL;        
        }
        last_export->next = (WpExportInstance *)self->buffer_free;  // update linked list
        
    }
    else{
        self->exports = (WpExportInstance *)self->buffer_free;
    }   

    self->export_count++;
    address = (WpExportInstance *)self->buffer_free;
    self->buffer_free += sizeof(WpExportInstance);               // Move the free pointer
    return address;         // Return the address of the allocated global

}

WpFunctionInstance * WpStoreAllocFunction(WpStore *self, WpModuleState *mod, WasmBinFuncType func_type, WasmBinFunction func)
{
    WpFunctionInstance new_func;
    WpFunctionInstance *last_func;  
    WpFunctionInstance * address; 

     //Check if there is enough free memory to allocate the global
    if (self->buffer_free + sizeof(WpFunctionInstance) > self->buffer + self->buffer_size)
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
    new_func.locals = func.locals;
    new_func.body = func.body;
    new_func.body_end = func.end;

    //alloc function
    memcpy(self->buffer_free, &new_func, sizeof(WpFunctionInstance));  // Allocate memory for function

    if(self->func_count > 0) { //Not first function
        last_func = WpStoreGetFunctionByIndex(self, self->func_count - 1);
        if(!last_func){
            return NULL;        
        }
        last_func->next = (WpFunctionInstance *)self->buffer_free;  // update linked list
        
    }
    else{
        self->funcs = (WpFunctionInstance *)self->buffer_free;
    }

    self->func_count++;
    address = (WpFunctionInstance *)self->buffer_free;
    self->buffer_free += sizeof(WpFunctionInstance);               // Move the free pointer
    return address;         // Return the address of the allocated global
}

