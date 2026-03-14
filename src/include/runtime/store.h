
#ifndef WASPC_RUNTIME_STORE_H
#define WASPC_RUNTIME_STORE_H

#ifdef __cplusplus
    extern "C" {
#endif

#include "webassembly/values.h"
#include "objects/wp_objects.h"


#include <stdint.h>

typedef struct WpStore{

    uint8_t *mem;                               ///pointer to static allocation for store area
    uint32_t mem_size;                          ///size of store area
    uint8_t *mem_free;                          ///pointer to static allocation for mark area

    WpGlobalInstance *globals;                  //head pointer to global linked list
    uint32_t global_count;

    WpFunctionInstance *funcs;                  //head pointer to function linked list
    uint32_t func_count;


}WpStore;


void WpStoreInit(WpStore *self);

WpGlobalInstance * WpStoreAllocGlobal(WpStore *self, uint8_t mut, StackValType type, StackValue val);

WpFunctionInstance * WpStoreAllocFunction(WpStore *self, WpModuleState *mod, WasmBinFuncType func_type, const uint8_t *locals, const uint8_t *body);

WpFunctionInstance * WpStoreGetFunctionByIndex(WpStore *self, uint32_t index);
#ifdef __cplusplus
    }
#endif

#endif





