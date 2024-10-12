/**
 * @file wasmbinmodule.h
 * @author Hector E. Socarras (hsocarras1987@gmail.com)
 * @brief Header file for waspc runtime base object.
 * @version 0.1
 * @date 2024-06-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef WASPC_OBJECT_WASM_MODULE_H
#define WASPC_OBJECT_WASM_MODULE_H

#ifdef __cplusplus
    extern "C" {
#endif

//wasp includes
#include "object/object.h"
#include "webassembly/structure/module.h"
#include "webassembly/structure/types.h"

#include <stdint.h>

/**
 * @brief 2.5
 * A module collects definitions for types, functions, tables, memories, and globals. In addition, it can declare imports
 * and exports and provide initialization in the form of data and element segments, or a start function.
 * module ::= { types vec(functype),
 *      funcs vec(func),
 *      tables vec(table),
 *      mems vec(mem),
 *      globals vec(global),
 *      elems vec(elem),
 *      datas vec(data),
 *      start start?,
 *      imports vec(import),
 *      exports vec(export) }
 * 
 */
typedef struct WasmModule{
    /// head for all Waspc object to allow cast
    WpObjectType type;

    // type  
    uint32_t type_len;    
    FuncType *types;

    // Functions
    uint32_t func_len;
    Func *funcs;

    //Tables
    uint32_t table_len;
    TableType *tables;

    //Mems
    uint32_t mem_len;
    MemType *mems;

    //Global
    uint32_t global_len;
    Global *globals;

    //Element
    uint32_t elem_len;
    Elem *elems; 

    //Data
    uint32_t data_count;
    Data *datas;

    //start
    uint32_t start;

    // import
    uint32_t imports_len;
    Import *imports;   

    //Export
    uint32_t exports_len;
    Export *exports;   
    

} WasmModule;

// Methods *****************************************************************************************
void ObjectWasmModuleInit(WasmModule *self);

#ifdef __cplusplus
    }
#endif

#endif