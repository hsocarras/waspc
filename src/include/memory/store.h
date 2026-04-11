
#ifndef WASPC_MEMORY_STORE_H
#define WASPC_MEMORY_STORE_H

#ifdef __cplusplus
    extern "C" {
#endif


#include "webassembly/bin.h"
#include "webassembly/values.h"
#include "objects/wp_objects.h"


#include <stdint.h>

typedef struct WpStore{

    uint8_t *buffer;                               ///pointer to static allocation for store area
    uint32_t buffer_size;                          ///size of store area
    uint8_t *buffer_free;                          ///pointer to static allocation for mark area

    WpGlobalInstance *globals;                  //head pointer to global linked list
    uint32_t global_count;

    WpFunctionInstance *funcs;                  //head pointer to function linked list
    uint32_t func_count;

    WpExportInstance *exports;                  //head pointer to export linked list
    uint32_t export_count;

    WpMemoryInstance *memories;                 //head pointer to memory linked list
    uint32_t memory_count;

}WpStore;


void WpStoreInit(WpStore *self);

WpFunctionInstance * WpStoreGetFunctionByIndex(WpStore *self, uint32_t index);

WpGlobalInstance * WpStoreAllocGlobal(WpStore *self, uint8_t mut, StackValType type, StackValue val);

WpMemoryInstance * WpStoreAllocMemory(WpStore *self, WasmBinMemory mem, uint8_t *data_memory);

WpExportInstance * WpStoreAllocExport(WpStore *self, WasmBinExport exp, WpModuleState *mod);

WpFunctionInstance * WpStoreAllocFunction(WpStore *self, WpModuleState *mod, WasmBinFuncType func_type, WasmBinFunction func);


#ifdef __cplusplus
    }
#endif

#endif





