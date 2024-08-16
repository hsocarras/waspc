/**
 * @file hash_table.h
 * @author Hector E. Socarras (hsocarras1987@gmail.com)
 * @brief  Header file with deficition for frame. That is main interpreter executable block.
 * @version 0.1
 * @date 2024-06-25
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef WASPC_MEMORY_ALLOC_H
#define WASPC_MEMORY_ALLOC_H

#ifdef __cplusplus
    extern "C" {
#endif

#include <stdint.h>

typedef struct Entry{
    uint8_t *heap;
    void * index;    
}WpAlloc;




void InitHashTable(HashTable *self);
void FreeHashTable(HashTable *self);

#ifdef __cplusplus
    }
#endif

#endif