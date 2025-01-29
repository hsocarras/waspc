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

#include "objects/module.h"

static void WasModuleInit(WasModule *self){
    
    self->types = (VecFuncTypes){0, NULL};
}

void WpModuleInit(WpModuleState *self){

    self->type = WP_OBJECT_MODULE;
    self->name = NULL;
    self->status = WP_MODULE_STATUS_INIT;

    ///Binary propertyes///////////////////////////////////////////////////////////////////
    self->buf = NULL;
    self->bufsize = 0;
    self->version = 0;
    self->typesec = (WasmBinSection){0, NULL};
    self->importsec = (WasmBinSection){0, NULL};
    self->functionsec = (WasmBinSection){0, NULL};
    self->tablesec = (WasmBinSection){0, NULL};
    self->memsec = (WasmBinSection){0, NULL};
    self->globalsec = (WasmBinSection){0, NULL};
    self->exportsec = (WasmBinSection){0, NULL};
    self->startsec = (WasmBinSection){0, NULL};
    self->elemsec = (WasmBinSection){0, NULL};
    self->datacountsec = (WasmBinSection){0, NULL};
    self->codesec = (WasmBinSection){0, NULL};
    self->datasec = (WasmBinSection){0, NULL};
    ////////////////////////////////////////////////////////////////////////////////////////

    WasModuleInit(&self->was);
}

