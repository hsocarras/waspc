
#ifndef WASPC_RUNTIME_STORE_H
#define WASPC_RUNTIME_STORE_H

#ifdef __cplusplus
    extern "C" {
#endif

#include "webassembly/values.h"
#include "objects/wp_objects.h"


#include <stdint.h>

typedef struct WpStore{

    uint8_t *mem;                              ///pointer to static allocation for store area
    uint32_t mem_size;                        ///size of store area
    uint8_t *mem_free;                            ///pointer to static allocation for mark area
}WpStore;


void WpStoreInit(WpStore *self);

const uint8_t * WpStoreAllocTypes(WpStore *self, const uint8_t *address);

uint8_t * WpStoreAllocGlobal(WpStore *self, uint8_t mut, WasValType type, WasValue val);

const uint8_t * WpStoreAllocFunction(WpStore *self, WpModuleState *mod, const uint8_t * func_type, const uint8_t *locals, const uint8_t *body);

#ifdef __cplusplus
    }
#endif

#endif





