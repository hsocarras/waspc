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

#ifndef WASPC_OBJECTS_GLOBAL_H
#define WASPC_OBJECTS_GLOBAL_H

#ifdef __cplusplus
    extern "C" {
#endif

//wasp includes
#include "objects/object.h"
#include "interpreter/values.h"

#include <stdint.h>

/*
 * @brief WpGlobalInstance represents a global instance in the WebAssembly module.
 */
typedef struct WpGlobalInstance {
    /// head for all Waspc object to allow cast
    WpObjectType wp_type;
    struct WpGlobalInstance *next;

    uint8_t imported;                           ///0 for not imported, 1 for imported
    struct WpGlobalInstance *address;           /// pointer to the global instance, assigned during module instantiation
    
    uint8_t mut;        ///0 for immutable, 1 for mutable
    StackValType type;    ///value type
    StackValue val;       ///curent value of the global

    

}WpGlobalInstance;



// Methods **************************************************************************************************
void WpGlobalInstanceInit(WpGlobalInstance *self);

/**
 * @brief Get the value of a global instance
 * 
 * If the global is imported, returns the value from the global instance pointed by address.
 * Otherwise, returns its own value.
 * 
 * @param self Pointer to the WpGlobalInstance object
 * @return StackValue The value of the global
 */
StackValue WpGlobalInstanceGetValue(WpGlobalInstance *self);

/**
 * @brief Set the value of a global instance
 * 
 * Checks if the global is mutable before setting the value.
 * If the global is imported, sets the value at the global instance pointed by address.
 * Otherwise, sets its own value.
 * 
 * @param self Pointer to the WpGlobalInstance object
 * @param value The new value to set
 * @return int 1 if the value was set successfully, 0 if the global is immutable
 */
int WpGlobalInstanceSetValue(WpGlobalInstance *self, StackValue value);


#ifdef __cplusplus
    }
#endif

#endif