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

//Types //////////////////////////////////////////////////////////////////////////////////////////////
typedef struct VecFuncType{

    // types  
    uint32_t lenght;    
    FuncType *elements;

} VecFuncType;

// Import //////////////////////////////////////////////////////////////////////////////////////////////


typedef struct Import{
    
    Name module;                    ///pointer to module name in binary file
    Name name;                      ///pointer to import name in binary file                          
    ExternalType type;              ///type clasifiying imports

    union desc{
        uint32_t x;
        TableType tt;
        MemType mt;
        GlobalType gt;
    } desc;

} Import;

typedef struct VecImport {

    // types  
    uint32_t lenght;    
    Import *elements;

}VecImport;
//#####################################################################################################


// FUNCTION ///////////////////////////////////////////////////////////////////////////////////////////
//The funcs component of a module defines a vector of functions with the following structure:
//  func ::= {type typeidx, locals vec(valtype), body expr}
typedef struct ExtLocal{
        uint8_t *val_type;      //val_type
        uint32_t *offset;       //Extended property, used to calculate local address in the value stack from frame satck pointer.
    } ExtLocal;                  

typedef struct Func{
    //type index
    FuncType * type;               
    //array of locals variables
    VecValType *locals;                  
    //function body
    Expr body;
} Func;

typedef struct VecFunc{

    // funcs
    uint32_t lenght;    
    Func *elements;

} VecFunc;
//#####################################################################################################
// Table //////////////////////////////////////////////////////////////////////////////////////////////
typedef TableType Table;

typedef struct VecTable{
    uint32_t lenght;
    Table *elements;
} VecTable;

// Memory //////////////////////////////////////////////////////////////////////////////////////////////
typedef struct VecMem{
    uint32_t lenght;
    MemType *elements;
} VecMem;

// Global ///////////////////////////////////////////////////////////////////////////////////////////
typedef struct Global{
    GlobalType gt;
    Expr e;    
} Global;

typedef struct VecGlobal{
    uint32_t lenght;
    Global *elements;
}VecGlobal;
//#####################################################################################################

// Export //////////////////////////////////////////////////////////////////////////////////////////////
typedef struct Export{    
    
    Name name;
    ExternalType type;
    union module
    {
        uint32_t func;
        uint32_t table;
        uint32_t mem;
        uint32_t global;
        uint32_t x; //any index of obove
    } desc;

} Export;

typedef struct VecExport{
    uint32_t lenght;
    Export *elements;
} VecExport;
//#####################################################################################################

// Element //////////////////////////////////////////////////////////////////////////////////////////////

typedef enum ElemMode {
    WP_WAS_STRUC_MOD_ELEMENT_MODE_PASSIVE,
    WP_WAS_STRUC_MOD_ELEMENT_MODE_ACTIVE,
    WP_WAS_STRUC_MOD_ELEMENT_MODE_DECLARATIVE,     
} ElemMode;

typedef struct ElemModeActive{
    uint32_t table_idx;
    Expr offset;
    const uint8_t *inst;
} ElemModeActive;

typedef struct Elem{  
    //type
    RefType type;
    //init expression
    VecExpr init;
    //mode
    ElemMode mode;
    //Active mode
    ElemModeActive active;

} Elem;

typedef struct VecElem{
    uint32_t lenght;
    Elem *elements;                         
} VecElem;
//#####################################################################################################

// Code //////////////////////////////////////////////////////////////////////////////////////////////

typedef struct Locals{
    uint32_t n;                             //number of local variables
    ValType t;                              //local variable type
} Locals;

typedef struct VecLocals{
    uint32_t lenght;
    Locals *elements;
} VecLocals;

typedef struct CodeFunc{    
    VecLocals locals;
    Expr e;
} CodeFunc;

typedef struct Code{
    uint32_t size;
    CodeFunc code;
} Code;

// Data //////////////////////////////////////////////////////////////////////////////////////////////

typedef enum DataMode {
    WP_WAS_STRUC_MOD_DATA_MODE_PASSIVE,
    WP_WAS_STRUC_MOD_DATA_MODE_ACTIVE,        
} DataMode;

typedef struct DataModeActive{

    uint32_t memory;
    Expr offset;
}DataModeActive;

typedef struct Data{
    VecByte init;

    DataMode mode;

    DataModeActive active;
} Data;

typedef struct VecData{
    uint32_t length;
    Data *elements;
}VecData;
//#####################################################################################################


/**
 * @brief Module acording to webassembly spec 2.5
 * 
 */
typedef struct WasModule{

    // types  
    VecFuncType types;
    //Imports
    VecImport imports;
    //Functions
    VecFunc funcs;
    //Tables
    VecTable tables;
    //Memories
    VecMem mems;
    //Globals
    VecGlobal globals;
    //Exports
    VecExport exports;
    //Start
    uint32_t start;
    //Elements
    VecElem elem;
    //Data
    uint32_t data_count;
    VecData data;

}WasModule;




#ifdef __cplusplus
    }
#endif

#endif