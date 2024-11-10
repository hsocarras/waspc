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

#include <stdlib.h>
#include <stdint.h>


#define ALLOCATE(type, count) (type*)malloc(sizeof(type) * (count));

#define FREE_MEM(ptr) free(ptr);

#define GROW_ARRAY(type, pointer, oldCount, newCount) \
    (type*)Reallocate(pointer, sizeof(type) * (oldCount), sizeof(type) * (newCount))


void * Reallocate (void* pointer, size_t oldSize, size_t newSize);

#ifdef __cplusplus
    }
#endif

#endif