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
#include "runtime/alloc.h"

#include <stdlib.h>

/**
 * @brief Allocates and initializes a new function instance for a WebAssembly function.
 *
 * This function creates a new WpFunctionInstance structure, sets its type, arity, associated module,
 * and code pointer based on the provided function and module instance. The returned instance must be freed by the caller.
 *
 * @param f The function definition to instantiate.
 * @param mod_inst Pointer to the module instance to which this function belongs.
 * @return WpFunctionInstance* Pointer to the newly allocated function instance, or NULL on allocation failure.
 */
WpFunctionInstance * WpAllocFunction(Func f, WpModuleInstance *mod_inst){

    WpFunctionInstance *f_instance = malloc(sizeof(WpFunctionInstance));    
    if(!f_instance){
        return NULL;
    }
    f_instance->type = WP_OBJECT_FUNCTION_INSTANCE;
    f_instance->arity = &f.type;
    f_instance->module = mod_inst;
    f_instance->code= &f;

    return f_instance;
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
 */
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
}