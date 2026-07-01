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

#include <stddef.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>

#define WP_STORE_ALIGNMENT 8u

#define WP_STORE_ALIGN_SIZE(size) (((size) + (WP_STORE_ALIGNMENT - 1u)) & ~(WP_STORE_ALIGNMENT - 1u))

#define WP_STORE_ALIGN_POINTER(ptr) (((uintptr_t)(ptr) + (WP_STORE_ALIGNMENT - 1u)) & ~(uintptr_t)(WP_STORE_ALIGNMENT - 1u))

/**
 * @brief Allocates a block of memory from the store with the specified size, ensuring proper alignment.
 * The function checks if there is enough free space in the store's buffer to accommodate the requested size after alignment.
 * If successful, it updates the buffer_free pointer to reflect the allocated space and returns
 * @param self Pointer to the store from which to allocate memory.
 * @param size The size of the memory block to allocate in bytes.
 * @return A pointer to the allocated memory block, or NULL if there is not enough space in the store.
 */
static uint8_t *WpStoreAllocAligned(WpStore *self, size_t size)
{
    uint8_t *address;
    uint8_t *end;
    size_t alloc_size;

    // Ensure the store has been initialized and has a buffer to allocate from
    if(!self->buffer || !self->buffer_free)
    {
        return NULL;
    }

    address = (uint8_t *)WP_STORE_ALIGN_POINTER(self->buffer_free);   // Align the free pointer
    end = self->buffer + self->buffer_size;
    alloc_size = WP_STORE_ALIGN_SIZE(size);

    if(address > end || (size_t)(end - address) < alloc_size)
    {
        return NULL;
    }

    self->buffer_free = address + alloc_size;
    return address;
}

/**
 * @brief Initializes the store with a given memory size.
 */
void WpStoreInit(WpStore *self)
{
    self->buffer = NULL;
    self->buffer_size = 0;
    self->buffer_free = NULL; // Initially, the entire memory is free

    self->modules = NULL;
    self->module_count = 0;

    self->def_types = NULL;
    self->def_type_count = 0;

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

WpModuleInstance * WpStoreGetModuleByIndex(WpStore *self, uint32_t index)
{
    if(!self->modules)
    {
        return NULL;
    }

    WpModuleInstance *current = self->modules;

    for (size_t i = 0; i < index; i++)       
    {
        current = current->next;
    }
    return current;
}   

WpWasDefType * WpStoreGetDefTypeByIndex(WpStore *self, uint32_t index)
{
    if(!self->def_types)
    {
        return NULL;
    }

    WpWasDefType *current = self->def_types;
    for (size_t i = 0; i < index; i++)
    {
        current = current->next;
    }
    return current;
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
    return (WpFunctionInstance *)current;
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
/**
 * @brief Allocates a new module instance in the store and returns a pointer to it.
 *  The function takes a pointer to the store and a pointer to the module instance to be allocated. It first allocates memory for the new module instance using the WpStoreAllocAligned function, then copies the contents of the provided module instance into the newly allocated memory. Finally, it updates the linked list of modules in the store and increments the module count before returning a pointer to the allocated module instance.
 * @param self Pointer to the store where the module instance will be allocated.
 * @param mod Pointer to the module instance to be allocated.
 * @return Pointer to the allocated module instance, or NULL if allocation fails.
 */
WpModuleInstance * WpStoreAllocModule(WpStore *self, WpModuleInstance *mod)
{
    WpModuleInstance *last_mod;  
    WpModuleInstance * address;
    address = (WpModuleInstance *)WpStoreAllocAligned(self, sizeof(WpModuleInstance));
    if(!address)
    {
        return NULL;
    }
    
    //alloc module
    memcpy(address, mod, sizeof(WpModuleInstance));  // Allocate memory for module
    if(self->module_count > 0) { //Not first module   
        last_mod = WpStoreGetModuleByIndex(self, self->module_count - 1);
        if(!last_mod){
            return NULL;
        } 
        last_mod->next = address;  // update linked list
        
    }
    else{
        self->modules = address;
    }

    self->module_count++;

    return address;
}

/**
 * @brief Allocates a new defined type instance in the store and returns a pointer to it.
 * The function takes a pointer to the store and a pointer to the defined type instance to be allocated. 
 * It first allocates memory for the new defined type instance using the WpStoreAllocAligned function, 
 * then copies the contents of the provided defined type instance into the newly allocated memory. Finally,
 * it updates the linked list of defined types in the store and increments the defined type count before returning a pointer to the allocated defined type instance.
 * @param self Pointer to the store where the defined type instance will be allocated.
 * @param def Pointer to the defined type instance to be allocated.
 * @return Pointer to the allocated defined type instance, or NULL if allocation fails.
 */
WpWasDefType * WpStoreAllocDefType(WpStore *self, WpWasDefType * def)
{
    WpWasDefType *last_def;  
    WpWasDefType * address;
    address = (WpWasDefType *)WpStoreAllocAligned(self, sizeof(WpWasDefType));
    if(!address)
    {
        return NULL;
    }
    
    //alloc module
    memcpy(address, def, sizeof(WpWasDefType));  // Allocate memory for module
    if(self->def_type_count > 0) { //Not first module   
        last_def = WpStoreGetDefTypeByIndex(self, self->def_type_count - 1);
        if(!last_def){
            return NULL;
        } 
        last_def->next = address;  // update linked list
        
    }
    else{
        self->def_types = address;
    }

    self->def_type_count++;

    return address;

}

/**
 * @brief Allocates a new global instance in the store and returns a pointer to it.
 * The function takes a pointer to the store and a pointer to the global instance to be allocated.
 * @param self Pointer to the store where the global instance will be allocated.
 * @param global Pointer to the global instance to be allocated.
 * @return Pointer to the allocated global instance, or NULL if allocation fails.
 * The function first allocates memory for the new global instance using the WpStoreAllocAligned function,
 *  then copies the contents of the provided global instance into the newly allocated memory. Finally, it
 * updates the linked list of globals in the store and increments the global count before returning a pointer
 * to the allocated global instance.
 */
WpGlobalInstance * WpStoreAllocGlobal(WpStore *self, WpGlobalInstance *global)
{
    
    WpGlobalInstance *last_global;  
    WpGlobalInstance * address;     
   
    // Allocate memory for new global instance
    address = (WpGlobalInstance *)WpStoreAllocAligned(self, sizeof(WpGlobalInstance));
    if(!address)
    {
        return NULL;
    }   
    
    // alloc new global on store
    memcpy(address, global, sizeof(WpGlobalInstance)); 
    
    if(self->global_count > 0) { //Not first global
        last_global = WpStoreGetGlobalByIndex(self, self->global_count - 1); //get last global
        if(!last_global){
            return NULL;
        }
        last_global->next = address;  // update linked list        
    }
    else{
        self->globals = address;
    }

    self->global_count++;
    return address; // Return the address of the allocated global
}

/** 
 * @brief Allocates a new memory instance in the store and returns a pointer to it.
 * The function takes a pointer to the store, a pointer to the memory instance to be allocated,
 *  and a pointer to the data memory.
 * @param self Pointer to the store where the memory instance will be allocated.
 * @param mem Pointer to the memory instance to be allocated.
 * @param data_memory Pointer to the data memory for the new memory instance.
 * @return Pointer to the allocated memory instance, or NULL if allocation fails.
 */
WpMemoryInstance * WpStoreAllocMemory(WpStore *self, WpMemoryInstance *mem, uint8_t *data_memory)
{
    
    WpMemoryInstance *last_mem;  
    WpMemoryInstance * address;

    if(self->memory_count > 0) { //Not first memory
        last_mem = WpStoreGetMemoryByIndex(self, self->memory_count - 1);
        if(!last_mem){
            return NULL;
        }
    }

    address = (WpMemoryInstance *)WpStoreAllocAligned(self, sizeof(WpMemoryInstance));
    if(!address)
    {
        return NULL;
    }

    
    //alloc memory
    memcpy(address, mem, sizeof(WpMemoryInstance));  // Allocate memory for memory
    address->bytes = data_memory; // assign data memory to memory instance
    if(self->memory_count > 0) { //Not first memory    
        last_mem = WpStoreGetMemoryByIndex(self, self->memory_count - 1);
        if(!last_mem){
            return NULL;
        }
        last_mem->next = address;  // update linked list
        
    }
    else{
        self->memories = address;
    }

    self->memory_count++;
    return address;         // Return the address of the allocated global

}

/**
 * @brief Allocates a new export instance in the store and returns a pointer to it.
 * @param self Pointer to the store where the export instance will be allocated.
 * @param exp Pointer to the export instance to be allocated.
 * @return Pointer to the allocated export instance, or NULL if allocation fails.
 */
WpExportInstance * WpStoreAllocExport(WpStore *self, WpExportInstance *exp)
{
    
    WpExportInstance *last_export;  
    WpExportInstance * address;
    
    
    address = (WpExportInstance *)WpStoreAllocAligned(self, sizeof(WpExportInstance));
    if(!address)
    {
        return NULL;
    }

    //alloc export
    memcpy(address, exp, sizeof(WpExportInstance));  // Allocate memory for export

    if(self->export_count > 0) { //Not first export
        last_export = WpStoreGetExportByIndex(self, self->export_count - 1);
        if(!last_export){
            return NULL;
        }    
        last_export->next = address;  // update linked list
        
    }
    else{
        self->exports = address;
    }   

    self->export_count++;
    return address;         // Return the address of the allocated global

}

/**
 * @brief Allocates a new function instance in the store and returns a pointer to it.
 * @param self Pointer to the store where the function instance will be allocated.
 * @param func Pointer to the function instance to be allocated.
 * @return Pointer to the allocated function instance, or NULL if allocation fails.
 * The function first allocates memory for the new function instance using the WpStoreAllocAligned function, 
 * then copies the contents of the provided function instance into the newly allocated memory. Finally, it
 * updates the linked list of functions in the store and increments the function count before returning a pointer
 * to the allocated function instance.
 */
WpFunctionInstance * WpStoreAllocFunctionInstance(WpStore *self, WpFunctionInstance *func)
{
    WpFunctionInstance new_func;
    WpFunctionInstance *last_func;  
    WpFunctionInstance * address; 

     //Check if there is enough free memory to allocate the global
    if (self->buffer_free + sizeof(WpFunctionInstance) > self->buffer + self->buffer_size)
    {
        return NULL; // Not enough memory
    }

    address = (WpFunctionInstance *)WpStoreAllocAligned(self, sizeof(WpFunctionInstance));
    if(!address)
    {
        return NULL;
    }


    //alloc function
    memcpy(address, func, sizeof(WpFunctionInstance));  // Allocate memory for function

    if(self->func_count > 0) { //Not first function
        last_func = WpStoreGetFunctionByIndex(self, self->func_count - 1);
        if(!last_func){
            return NULL;
        }
        last_func->next = address;  // update linked list
        
    }
    else{
        self->funcs = address;
    }

    self->func_count++;    
    return address;         // Return the address of the allocated global
}

