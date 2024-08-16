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

/**
 * @brief Object for keep track for pou's source loaded in load memory
 * 
 */
typedef struct LoadBlock{

    char name[32];          /// pou's name TODO char pointer
    uint32_t size;          /// pou's bin len
    uint8_t *start;         /// where the pou start in load memory
    //LoadBlock *next;
    
} LoadBlock;

/**
 * @brief Object for managed load memory
 * 
 */
typedef struct LoadMemoryManager {
    BaseMemory bank;
    uint8_t *index;                 // pointer to where start next free space
    uint32_t block_count;           // counter to keep track LoadBlock in load memory

}LoadMemoryManager;

/**
 * @brief Constructor
 * 
 * @param self 
 * @param buf pointer to runtime's load memory
 * @param size Load memory size 
 */
void InitLoadMemoryManager(LoadMemoryManager *self, uint8_t *buf, const uint32_t size);

uint32_t GetFreeLoadMemory (const LoadMemoryManager *self);

float GetLoadMemoryUsage(const LoadMemoryManager *self);

WpError AppendBufferToLoadMem(LoadMemoryManager *self, const uint8_t *buf, const uint32_t size);

#ifdef __cplusplus
    }
#endif

#endif