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

typedef enum ImportDescType {
    func,
    table,
    mem,
    global,
} ImportDescType;

typedef struct ImportDescBase {
    ImportDescType type;
} ImportDescBase;

typedef struct ImportDescFunction {
    ImportDescType type;
    TypeIdx func;
} ImportDescBase;

typedef struct ImportDescTable {
    ImportDescType type;
    TableType table;
} ImportDescTable;

typedef struct ImportDescMem {
    ImportDescType type;
    MemType mem;
} ImportDescMem;

typedef struct ImportDescGlobal {
    ImportDescType type;
    GlobalType global;
} ImportDescGlobal;

typedef struct WasmModuleImport{
    
    uint32_t name_len;
    char *module;                   ///module name
    ImportDescBase *desc;

} WasmModuleImport;


typedef struct WasmModule{
    
    uint32_t *imports_len;
    WasmModuleImport *imports;
} WasmModule;



#ifdef __cplusplus
    }
#endif

#endif