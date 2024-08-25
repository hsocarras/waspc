/**
 * @file 
 * @author Hector E. Socarras (hsocarras1987@gmail.com)
 * @brief Header file for wasm load module
 * @version 0.1
 * @date 2024-06-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef WASPC_MEMORY_LOAD_MEMORY_H
#define WASPC_MEMORY_LOAD_MEMORY_H

#ifdef __cplusplus
    extern "C" {
#endif

//wasp includes
#include "memory/memory.h"
#include "diagnostic/error.h"

#include <stdint.h>

typedef struct WpArena {

    uint64_t used_pools;             ///mark wich one of the 64 pool are used puting coresponding bit to one
    uint64_t free_pools;             ///mark wich one of the 64 pool are used puting coresponding bit to one

} WpArena;

#ifdef __cplusplus
    }
#endif

#endif