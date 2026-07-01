/**
 * @file instance.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef WASPC_OBJECTS_MODULE_INSTANCE_H
#define WASPC_OBJECTS_MODULE_INSTANCE_H

#ifdef __cplusplus
    extern "C" {
#endif

//wasp includes
#include "objects/object.h"
#include "objects/function.h"
#include "objects/global.h"
#include "objects/memory.h"
#include "objects/export.h"
#include "objects/was_deftype.h"
#include "webassembly/bin.h"   



#include <stdint.h>

/**
 * @version 3.0
 * @brief Result object for return values for most waspc internal functions.
 * 
 */
typedef struct WpModuleInstance{
    /// head for all Waspc object to allow cast
    WpObjectType wp_type;     

    
    
    //WasModule's counter for instantiation; 
    uint32_t def_function_count;       /// counter for the number of defined functions in the module, defined at function section.
    uint32_t import_count;             /// number of imports in the module
    uint32_t table_count;              /// number of tables in the module
    uint32_t tag_count;
    uint32_t element_count;            /// number of elements in the module
    uint32_t data_count;               /// number of data segments in the module
    void *tables;

    
    /// @brief Instances ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
    WpWasDefType *types;                    /// pointer to the defined types for the module instance, these are created from the type section of the module binary file.  
    uint32_t type_count;                /// number of function types in the module
    WpGlobalInstance *globals;                  /// pointer to the global instances
    uint32_t global_count;              /// number of globals in the module    
    WpFunctionInstance *funcs;                    /// pointer to the function instances
    uint32_t function_count;            /// number of functions in the module
    WpMemoryInstance *mems;                     /// pointer to the memory instances
    uint32_t mem_count;                 /// number of memories in the module
    WpExportInstance *exports;                  /// pointer to the export instances
    uint32_t export_count;              /// number of exports in the module
    
    //Start function index.
    uint32_t start_func_index;        /// index of the start function in the function section, if there is no start function, this value is 0xFFFFFFFF

    struct WpModuleInstance *next; /// pointer to next module instance in the store. This is for internal use only, not for public API.

} WpModuleInstance;

// Methods **************************************************************************************************

void WpModuleInstanceInit(WpModuleInstance *self);



#ifdef __cplusplus
    }
#endif

#endif