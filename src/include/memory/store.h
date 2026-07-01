
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

    WpModuleInstance *modules;                  //head pointer to module linked list
    uint32_t module_count;

    WpWasDefType *def_types;                 //head pointer to defined type linked list
    uint32_t def_type_count;

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

WpModuleInstance * WpStoreAllocModule(WpStore *self, WpModuleInstance *mod);

WpWasDefType * WpStoreAllocDefType(WpStore *self, WpWasDefType * def);

WpGlobalInstance * WpStoreAllocGlobal(WpStore *self, WpGlobalInstance *global);

WpMemoryInstance * WpStoreAllocMemory(WpStore *self, WpMemoryInstance *mem, uint8_t *data_memory);

WpExportInstance * WpStoreAllocExport(WpStore *self, WpExportInstance *exp);

WpFunctionInstance * WpStoreAllocFunctionInstance(WpStore *self, WpFunctionInstance *func);

#ifdef __cplusplus
    }
#endif

#endif





