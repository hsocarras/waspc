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
#include "object/error.h"

#include <stdint.h>

/**
 * @brief Object for keep track for pou's source loaded in load memory
 * 
 *
typedef struct WasmLoadInfo{

    uint32_t id;          /// pou's hash name
    uint32_t size;          /// pou's bin len
    uint8_t *start;         /// where the pou start in load memory
        
} WasmLoadInfo;*/

/**
 * @brief Object for managed load memory
 * 
 *
typedef struct WorkMemory {
    uint32_t code_size;     
    uint8_t *code;                  /// buffer to stored WasmBinModule
    uint8_t *index;                 // pointer to where start next free space
    uint32_t block_count;           // counter to keep track LoadBlock in load memory

}WorkMemory;*/

/**
 * @brief Constructor
 * 
 * @param self 
 * @param buf pointer to runtime's load memory
 * @param size Load memory size 
 */
//void InitWorkMemory(WorkMemory *self);

//void InitCodeMem(WorkMemory *self, uint8_t * buf, uint32_t size);

//uint32_t GetFreeCodeMem (const WorkMemory *self);

//float GetLoadMemoryUsage(const WorkMemory *self);

//WpError AppendWasmCode(WorkMemory *self, const uint8_t *buf, const uint32_t size);

#ifdef __cplusplus
    }
#endif

#endif