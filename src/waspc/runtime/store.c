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
#include "webassembly/values.h"

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

const uint8_t * WpStoreAllocTypes(WpStore *self, const uint8_t *address)
{
    //Check if there is enough free memory to allocate the types
    if (self->mem_free + sizeof(uint8_t *) > self->mem + self->mem_size)
    {
        return NULL; // Not enough memory
    }
    memcpy(self->mem_free, &address, sizeof(uint8_t *)); // Allocate memory for types
    self->mem_free += sizeof(uint8_t *); // Move the free pointer
    return self->mem_free - sizeof(uint8_t *); // Return the address of the allocated types
}

uint8_t * WpStoreAllocGlobal(WpStore *self, uint8_t mut, WasValType type, WasValue val)
{
    WpGlobalInstance global;
    WpGlobalInstanceInit(&global);
    //Check if there is enough free memory to allocate the global
    if (self->mem_free + sizeof(WpGlobalInstance) > self->mem + self->mem_size)
    {
        return NULL; // Not enough memory
    }
    
    global.mut = mut;
    global.type = type;
    global.val = val;

    memcpy(self->mem_free, &global, sizeof(WpGlobalInstance)); // Allocate memory for global
    self->mem_free += sizeof(WpGlobalInstance); // Move the free pointer
    return self->mem_free - sizeof(WpGlobalInstance); // Return the address of the allocated global
}
   
const uint8_t * WpStoreAllocFunction(WpStore *self, WpModuleState *mod, const uint8_t * func_type, const uint8_t *locals, const uint8_t *body)
{
    WpFunctionInstance func;
    WpFunctionInstanceInit(&func);

     //Check if there is enough free memory to allocate the global
    if (self->mem_free + sizeof(WpFunctionInstance) > self->mem + self->mem_size)
    {
        return NULL; // Not enough memory
    }

    func.module = mod;
    func.func_type = func_type;
    func.locals = locals;
    func.body = body;

    memcpy(self->mem_free, &func, sizeof(WpFunctionInstance));  // Allocate memory for function
    self->mem_free += sizeof(WpFunctionInstance);               // Move the free pointer
    return self->mem_free - sizeof(WpFunctionInstance);         // Return the address of the allocated global
}

/**
 * @brief Allocates and initializes a new export instance for a WebAssembly export.
 *
 * This function creates a new WpExportInstance structure, sets its type, name, kind,
 * and value based on the provided export definition and associated value.
 * The returned instance must be freed by the caller.
 *
 * @param export_def The export definition to instantiate.
 * @param value Pointer to the value (function, table, memory, or global) being exported.
 * @return WpExportInstance* Pointer to the newly allocated export instance, or NULL on allocation failure.
 *
WpExportInstance * WpAllocExportInstance(Export export_def, void *value) {
    WpExportInstance *exp_instance = malloc(sizeof(WpExportInstance));
    if (!exp_instance) {
        return NULL;
    }
    exp_instance->type = WP_OBJECT_EXPORT_INSTANCE;
    exp_instance->name = export_def.name;
    exp_instance->export_type = export_def.type;

    // Set the value based on the export type
    switch (export_def.type) {
        case WP_EXTERNAL_TYPE_FUNC:
            exp_instance->value.func = (funcaddr)value; // Cast value to funcaddr
            break;
        case WP_EXTERNAL_TYPE_TABLE:
            // Handle table export if needed
            break;
        case WP_EXTERNAL_TYPE_MEMORY:
            // Handle memory export if needed
            break;
        case WP_EXTERNAL_TYPE_GLOBAL:
            // Handle global export if needed
            break;
        default:
            free(exp_instance);
            return NULL; // Invalid export type
    }
    
    return exp_instance;
}*/