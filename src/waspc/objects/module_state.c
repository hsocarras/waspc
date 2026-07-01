/**
 * @file module.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-01-26
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "objects/module_state.h"
#include <stdlib.h>


/**
 * @version 3.0
 * @brief Initialize a module state object
 */
void WpModuleStateInit(WpModuleState *self){

    self->wp_type = WP_OBJECT_MODULE_STATE;
    self->status = WP_MODULE_STATUS_INIT;

    ///Binary propertyes///////////////////////////////////////////////////////////////////
    self->buf = NULL;
    self->bufsize = 0;
    self->version = 0;

    //Binary sections start and size, these are for internal use only, not for public API.
    self->typesec = (WasmBinSection){0, NULL};
    self->importsec = (WasmBinSection){0, NULL};
    self->functionsec = (WasmBinSection){0, NULL};
    self->tablesec = (WasmBinSection){0, NULL};
    self->memsec = (WasmBinSection){0, NULL};
    self->tagsec = (WasmBinSection){0, NULL};
    self->globalsec = (WasmBinSection){0, NULL};
    self->exportsec = (WasmBinSection){0, NULL};
    self->startsec = (WasmBinSection){0, NULL};
    self->elemsec = (WasmBinSection){0, NULL};
    self->datacountsec = (WasmBinSection){0, NULL};
    self->codesec = (WasmBinSection){0, NULL};
    self->datasec = (WasmBinSection){0, NULL};
    ////////////////////////////////////////////////////////////////////////////////////////    
    self->instance = NULL;
    ////////////////////////////////////////////////////////////////////////////////////////
    
}

