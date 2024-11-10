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

typedef struct Import{
    
    uint32_t module_len;
    const unsigned char *module;                   ///pointer to module name in binary file
    uint32_t name_len;
    const unsigned char *name;                      ///pointer to import name in binary file
    ImportDesc *desc;

} Import;
//#####################################################################################################


// FUNCTION ///////////////////////////////////////////////////////////////////////////////////////////
//The funcs component of a module defines a vector of functions with the following structure:
//  func ::= {type typeidx, locals vec(valtype), body expr}
typedef struct Func{
    uint32_t type;               //type index
    uint32_t local_len;         //How many locals are    
    uint8_t *locals;           //array with the type of each local vaiable 
    uint32_t body_len;
    const uint8_t *body;
} Func;
//#####################################################################################################


// Global ///////////////////////////////////////////////////////////////////////////////////////////
typedef struct Global{
    uint8_t type;
    uint8_t mut;    
    uint32_t expr_len;
    const uint8_t *expr;    
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

typedef struct Export{    
    
    uint32_t name_len;
    const unsigned char *name;
    ExportDesc *desc;

} Export;
//#####################################################################################################

// Element //////////////////////////////////////////////////////////////////////////////////////////////

typedef enum ElemMode {
    WP_WAS_STRUC_MOD_ELEMENT_MODE_PASSIVE,
    WP_WAS_STRUC_MOD_ELEMENT_MODE_ACTIVE,
    WP_WAS_STRUC_MOD_ELEMENT_MODE_DECLARATIVE,     
} ElemMode;

typedef struct Elem{    
    
    ElemMode mode;
    uint8_t type;
    uint32_t table_idx;             //table index

    uint32_t offset_len;            //table offset expresion
    const uint8_t *offset;          //table offset expresion

    uint32_t init_len;
    const uint8_t *init;    
    
    

} Elem;
//#####################################################################################################

// Data //////////////////////////////////////////////////////////////////////////////////////////////

typedef enum DataMode {
    WP_WAS_STRUC_MOD_DATA_MODE_PASSIVE,
    WP_WAS_STRUC_MOD_DATA_MODE_ACTIVE,        
} DataMode;

typedef struct Data{
    DataMode mode; 
    uint32_t mem_idx;
    uint32_t offset_len;
    const uint8_t * offset;  
    uint32_t init_len;
    const uint8_t *init;
} Data;
//#####################################################################################################






#ifdef __cplusplus
    }
#endif

#endif