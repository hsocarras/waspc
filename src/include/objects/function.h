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
#include "objects/was_deftype.h" 
#include "interpreter/values.h"
// Forward declaration of ModuleInstance
struct WpModuleInstance;

#include <stdint.h>

typedef StackValue (*HostFunc)(uint32_t argCount, StackValue* args);

typedef enum {
    WP_FUNC_NORMAL,
    WP_FUNC_IMPORT,
    WP_FUNC_HOST,
} WpFunctionType;

typedef struct WpBuildinFunction {
    WpObjectType wp_type;
    const WpWasDefType *func_type;  // Pointer to the function signature in the signatures array
    HostFunc func_ptr;              // Pointer to the host function implementation
} WpBuildinFunction;

/**
 * @brief WpFunctionInstance represents a function instance in the WebAssembly module.
 */
typedef struct WpFunctionInstance {
    /// head for all Waspc object to allow cast
    WpObjectType wp_type;
    struct WpFunctionInstance *next;

    WpFunctionType func_kind;                   /// type of the function, used for type checking in module instantiation
    struct WpFunctionInstance *address;         /// pointer to the function instance, assigned during module instantiation
    HostFunc host_func;                         /// pointer to the host function, used for calling host functions

    /// @brief module instance that function belong to
    struct WpModuleInstance *module;
    
    //Destructure type
    const WpWasDefType *func_type;                 /// pointer to the function type in store.

    /// @brief code.    
    const uint8_t * locals;             /// pointer to locals start in binary file, it can be used to get the local types without decode them into WasmValueType.
    const uint8_t *body;                /// pointer to the function code's start. instructions* .
    const uint8_t *body_end;            /// pointer to the function instruction's end 0x0B opcode.

    
}WpFunctionInstance;

// Methods **************************************************************************************************

void WpFunctionInstanceInit(WpFunctionInstance *self, WpFunctionType func_kind);

#ifdef __cplusplus
    }
#endif

#endif