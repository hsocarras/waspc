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

#ifndef WASPC_MEMORY_MEMORY_H
#define WASPC_MEMORY_MEMORY_H

#ifdef __cplusplus
    extern "C" {
#endif

//wasp includes
//Will be remplaced for something similar to python's arena, pool, block memory manager scheme.


#include <stdint.h>



/**
 * @brief TODO base memory
 * 
 */
typedef struct BaseMemory {
    uint32_t size;              //buffer size in bytes
    uint8_t *buf;               //pointer to an allocated byte array
}BaseMemory;

#ifdef __cplusplus
    }
#endif

#endif