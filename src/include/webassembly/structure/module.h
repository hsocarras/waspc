/**
 * @file module.h
 * @author Hector E. Socarras (hsocarras1987@gmail.com)
 * @brief Header file for webassembly virtual machine
 * @version 0.1
 * @date 2024-06-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef WASPC_WEBASSEMBLY_STRUCTURE_MODULE_H
#define WASPC_WEBASSEMBLY_STRUCTURE_MODULE_H

#ifdef __cplusplus
    extern "C" {
#endif

#include "webassembly/structure/types.h"

#include <stdint.h>


// Types //////////////////////////////////////////////////////////////////////////////////////////////
/*
typedef struct WasmModuleType{    
    uint32_t type_len;    
    FuncType *type;
} WasmModuleType;*/
//#####################################################################################################


// Import //////////////////////////////////////////////////////////////////////////////////////////////
typedef enum ImportDescType {
    FUNC,
    TABLE,
    MEM,
    GLOBAL,
} ImportDescType;

typedef struct ImportDesc {
    ImportDescType type;
    uint32_t idx;               //index defined at 2.5.1 Indices   
} ImportDesc;

typedef struct WasmModuleImport{
    
    uint32_t module_len;
    char *module;                   ///module name
    uint32_t name_len;
    char *name;
    ImportDescBase *desc;

} WasmModuleImport;
//#####################################################################################################

typedef struct WasmModule{

    // type section. 
    uint32_t type_len;    
    FuncType *types;

    uint32_t *imports_len;
    WasmModuleImport *imports;
} WasmModule;



#ifdef __cplusplus
    }
#endif

#endif