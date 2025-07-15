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
#include <stdlib.h>

static void WasModuleInit(WasModule *self){
    
    self->types = (VecFuncType){0, NULL};

    self->imports = (VecImport){0, NULL};

    self->funcs = (VecFunc){0, NULL};

    self->globals = (VecGlobal){0, NULL};

    self->tables = (VecTable){0, NULL};

    self->mems = (VecMem){0, NULL};

    self->exports = (VecExport){0, NULL};

    self->start = 0;

    self->elem = (VecElem){0, NULL};

    self->data_count = 0;

    self->data = (VecData){0, NULL};
}

void WpModuleInstanceInit(WpModuleInstance *self){
    self->type = WP_OBJECT_MODULE_INSTANCE;

    self->types = NULL;

    self->funcaddrs.lenght = 0;
    self->funcaddrs.elements = NULL;
    /*
    self->tableaddrs.lenght = 0;
    self->tableaddrs.elements = NULL;

    self->memaddrs.lenght = 0;
    self->memaddrs.elements = NULL;

    self->globaladdrs.lenght = 0;
    self->globaladdrs.elements = NULL;

    self->elemaddrs.lenght = 0;
    self->elemaddrs.elements = NULL;

    self->dataaddrs.lenght = 0;
    self->dataaddrs.elements = NULL;*/
}

void WpModuleInit(WpModuleState *self){

    self->type = WP_OBJECT_MODULE_STATE;
    self->name.name = NULL;
    self->name.lenght = 0;
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
    self->was = (WasModule *)malloc(sizeof(WasModule));
    if (!self->was) {
        // Handle memory allocation error
        self->type = WP_OBJECT_ERROR;
        self->status = WP_MODULE_STATUS_ERROR;
        return;
    }
    WasModuleInit(self->was);

    WpModuleInstanceInit(&self->instance);


}

