# ifndef WASPC_RUNTIME_INSTANCES_H
# define WASPC_RUNTIME_INSTANCES_H  

#include "webassembly/values.h"
#include "objects/module.h"

#ifdef __cplusplus
    extern "C" {
#endif
#include "webassembly/values.h"
#include <stdint.h>


typedef struct WpGlobalInstance{
    uint8_t mut;
    WasValType type;
    WasValue val;
}WpGlobalInstance;

typedef struct WpFuncInstance{
    uint32_t typeidx;
    WpModuleState *mod_instance;
    const uint8_t *locals;
    const uint8_t *code;
}WpFuncInstance;


#ifdef __cplusplus
    }
#endif

#endif