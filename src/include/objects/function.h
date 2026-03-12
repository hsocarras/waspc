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

// Forward declaration of ModuleInstance
struct WpModuleState;

#include <stdint.h>


typedef struct WpFunctionInstance {
    /// head for all Waspc object to allow cast
    WpObjectType wp_type;

    /// type
    // deftype TODO
    /// @brief module instance that function belong to
    struct WpModuleState *module;
    /// @brief code.
    const uint8_t * func_type;          ///the function type in the module's type section. Signature.
    
    const uint8_t * locals;             /// pointer to locals start.
    const uint8_t *body;                /// pointer to the function code.

}WpFunctionInstance;

// Methods **************************************************************************************************

void WpFunctionInstanceInit(WpFunctionInstance *self);

#ifdef __cplusplus
    }
#endif

#endif