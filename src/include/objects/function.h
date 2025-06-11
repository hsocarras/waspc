/**
 * @file function.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-02-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */

 #ifndef WASPC_OBJECTS_FUNCTION_H
#define WASPC_OBJECTS_FUNCTION_H

#ifdef __cplusplus
    extern "C" {
#endif

//wasp includes
#include "objects/object.h"
#include "objects/module.h"
#include "webassembly/structure/module.h"

// Forward declaration of ModuleInstance
struct WpModuleInstance;

#include <stdint.h>


typedef struct WpFunctionInstance {
    /// head for all Waspc object to allow cast
    WpObjectType type;
    ///arity
    FuncType *arity;
    /// @brief module instance that function belong to
    struct WpModuleInstance *module;
    /// @brief webassembly function.
    Func *code;

}WpFunctionInstance;

typedef struct  VecFunctionInstace {
    uint32_t lenght;
    WpFunctionInstance *elements;
}VecFunctionInstace;

typedef WpFunctionInstance * funcaddr;

typedef struct VecFuncAddr{
    funcaddr *elements;
    uint32_t lenght;
} VecFuncAddr;


// Methods **************************************************************************************************

void WpFunctionInstanceInit(WpFunctionInstance *self);

#ifdef __cplusplus
    }
#endif

#endif