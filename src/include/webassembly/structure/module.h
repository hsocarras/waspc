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
    WP_WAS_STRUC_MOD_IMPORT_DESC_TYPE_FUNC,
    WP_WAS_STRUC_MOD_IMPORT_DESC_TYPE_TABLE,
    WP_WAS_STRUC_MOD_IMPORT_DESC_TYPE_MEM,
    WP_WAS_STRUC_MOD_IMPORT_DESC_TYPE_GLOBAL,
} ImportDescType;

typedef struct ImportDesc {
    ImportDescType type;
    uint32_t idx;               //index defined at 2.5.1 Indices   
} ImportDesc;

typedef struct WasmModuleImport{
    
    uint32_t module_len;
    const unsigned char *module;                   ///module name
    uint32_t name_len;
    const unsigned char *name;
    ImportDesc *desc;

} WasmModuleImport;
//#####################################################################################################


// FUNCTION ///////////////////////////////////////////////////////////////////////////////////////////
typedef struct Func{
    uint32_t idx;               //type index
    uint32_t local_len;         //How many locals are
    //uint32_t local_size;        //local's vector size in bytes because are diferen datatype
    uint8_t * local_types;      //array with the type of each local vaiable    
    ValType *locals;               //pointer to first local
    uint32_t body_len;
    const uint8_t *body;
} Func;
//#####################################################################################################

// TABLE //////////////////////////////////////////////////////////////////////////////////////////////
// Similar to dinamic array Lim is the capacity of table and usage is how many reference are.
//However dinamic reference array can't grow beyond of lim.max.
typedef struct Table{
    Limits lim;
    uint32_t usage;
    RefType *references;
} Table;
//#####################################################################################################

// MEM //////////////////////////////////////////////////////////////////////////////////////////////

typedef struct Mem{
    Limits lim;
    uint32_t usage;
    uint8_t *mem;
} Mem;
//#####################################################################################################

// Global ///////////////////////////////////////////////////////////////////////////////////////////
typedef struct Global{
    uint8_t type;
    uint8_t mut;
    ValType *value;
    uint32_t init_len;
    const uint8_t *init_code;    
} Global;
//#####################################################################################################

// Export //////////////////////////////////////////////////////////////////////////////////////////////
typedef enum ExportDescType {
    WP_WAS_STRUC_MOD_EXPORT_DESC_TYPE_FUNC,
    WP_WAS_STRUC_MOD_EXPORT_DESC_TYPE_TABLE,
    WP_WAS_STRUC_MOD_EXPORT_DESC_TYPE_MEM,
    WP_WAS_STRUC_MOD_EXPORT_DESC_TYPE_GLOBAL,
} ExportDescType;

typedef struct ExportDesc {
    ExportDescType type;
    uint32_t idx;               //index defined at 2.5.1 Indices   
} ExportDesc;

typedef struct WasmModuleExport{    
    
    uint32_t name_len;
    const unsigned char *name;
    ExportDesc *desc;

} WasmModuleExport;
//#####################################################################################################

// Element //////////////////////////////////////////////////////////////////////////////////////////////

typedef enum ElementMode {
    WP_WAS_STRUC_MOD_ELEMENT_MODE_PASSIVE,
    WP_WAS_STRUC_MOD_ELEMENT_MODE_ACTIVE,
    WP_WAS_STRUC_MOD_ELEMENT_MODE_DECLARATIVE,     
} ElementMode;

typedef struct WasmModuleElement{    
    
    RefType type;
    uint32_t init_len;
    const uint8_t *init;
    ElementMode mode;
    uint32_t table;     //table index
    uint32_t offset_len;
    uint8_t *offset;

} WasmModuleElement;
//#####################################################################################################


typedef struct WasmModule{

    // type  
    uint32_t type_len;    
    FuncType *types;

    // import
    uint32_t imports_len;
    WasmModuleImport *imports;

    // Functions
    uint32_t func_len;
    Func *funcs;

    //Tables
    uint32_t table_len;
    Table *tables;

    //Mems
    uint32_t mem_len;
    Mem *mems;

    //Global
    uint32_t global_len;
    Global *globals;

    //Export
    uint32_t exports_len;
    WasmModuleExport *exports;

    //start
    uint32_t start_function_index;

    //Element
    uint32_t element_len;
    WasmModuleElement *elements;

} WasmModule;



#ifdef __cplusplus
    }
#endif

#endif