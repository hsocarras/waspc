
#ifndef WASPC_RUNTIME_ALLOC_H
#define WASPC_RUNTIME_ALLOC_H

#ifdef __cplusplus
    extern "C" {
#endif

#include "webassembly/structure/module.h"
#include "objects/module.h"
#include "objects/function.h"
#include "objects/export.h"

WpFunctionInstance * WpAllocFunction(Func f, WpModuleInstance *mod_inst);

#ifdef __cplusplus
    }
#endif

#endif





