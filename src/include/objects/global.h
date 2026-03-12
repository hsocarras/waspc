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
#include "webassembly/values.h"

#include <stdint.h>


typedef struct WpGlobalInstance {
    /// head for all Waspc object to allow cast
    WpObjectType wp_type;
    
    uint8_t mut;        ///0 for immutable, 1 for mutable
    WasValType type;    ///value type
    WasValue val;       ///curent value of the global

}WpGlobalInstance;



// Methods **************************************************************************************************
void WpGlobalInstanceInit(WpGlobalInstance *self);


#ifdef __cplusplus
    }
#endif

#endif