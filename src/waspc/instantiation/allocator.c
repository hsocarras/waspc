/**
 * @file allocator.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-02-13
 * 
 * @copyright Copyright (c) 2025
 * 
 */
#include "webassembly/structure/module.h"
//#include "webassembly/execution/runtime/instances.h"
#include "webassembly/execution/runtime/addresses.h"
#include "objects/function.h"

WpFunctionInstance * WpFunctionInstanceAllocate(Func f, ModuleInstance *mod_inst){

    WpFunctionInstance *f_instance = malloc(sizeof(WpFunctionInstance));    
    if(!f_instance){
        return NULL;
    }
    f_instance->type = WP_OBJECT_FUNCTION_INSTANCE;
    f_instance->arity = &f.type;
    f_instance->module = mod_inst;
    f_instance->code= &f;

    return f_instance;
}