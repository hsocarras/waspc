/**
 * @file module_instance.h
 * @author Hector E. Socarras (hsocarras1987@gmail.com)
 * @brief Header file for webassembly module instances related to spec's runtime chapter
 * @version 0.1
 * @date 2024-06-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef WASPC_WEBASSEMBLY_RUNTIME_GLOBAL_INSTANCE_H
#define WASPC_WEBASSEMBLY_RUNTIME_GLOBAL_INSTANCE_H

#ifdef __cplusplus
    extern "C" {
#endif

#include "webassembly/structure/types.h"

#include <stdint.h>


typedef struct GlobalInstance {
    Mut mut;
    ValType value;    
} GlobalInstance;

typedef struct GlobalNumInstance {
    Mut mut;
    NumType value;  
} GlobalNumInstance;

typedef struct GlobalV128Instance {
    Mut mut;
    VecType value;  
} GlobalV128Instance;

typedef struct GlobalRefInstance {
    Mut mut;
    NumType value;  
} GlobalRefInstance;

Mut GetGlobalInstanceMutable(GlobalInstance *self);

ValType GetGlobalInstanceValue(GlobalInstance *self);

#ifdef __cplusplus
    }
#endif

#endif