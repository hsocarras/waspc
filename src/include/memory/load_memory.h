/**
 * @file
 * @author Hector E. Socarras (hsocarras1987@gmail.com)
 * @brief Header file for wasm load module.
 * Memory used to load, wasm file form rom. This memory should only be manipulated by the driver.
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
#include "objects/module_state.h"
#include "utils/hash_table.h"


#include <stddef.h>
#include <stdint.h>
#include <string.h>

#define WP_LOAD_MEMORY_ALIGMENT 64u
#define HASH_TABLE_MODULES_KEY_LEN 32

/**
 * @brief View of a WebAssembly binary file loaded in memory.
 */
typedef struct WpBinFile{
    const uint8_t *buf;
    uint32_t bufsize;
} WpBinFile;

typedef enum WpLoadMemoryError{
    WP_LOAD_MEMORY_OK = 0,
    WP_LOAD_MEMORY_ERR_INVALID_ARGUMENT = 1,
    WP_LOAD_MEMORY_ERR_OUT_OF_MEMORY = 2,
    WP_LOAD_MEMORY_ERR_TOO_MANY_SEGMENTS = 3,
    WP_LOAD_MEMORY_ERR_DUPLICATE_ID = 4,
    WP_LOAD_MEMORY_ERR_SEGMENT_NOT_FOUND = 5
} WpLoadMemoryError;

typedef struct HtModuleEntry {                            ///Entry for hash table    
    char key[HASH_TABLE_MODULES_KEY_LEN];                                       ///string key TODO config max character sisze. For now fixed to 32 characters
    WpModuleState module;                                    ///any kind value
} HtModuleEntry ;

/**
 * @brief Arena used by the embedder/driver to keep original WASM binaries.
 *
 * The arena does not own the buffer nor the segment table. Both must be provided
 * by the caller so the runtime can work with static memory if needed.
 */
typedef struct WpLoadMemory{
    uint8_t *buffer;                //static memory buffer
    uint32_t buffer_size;           //buffer's size
    uint32_t cursor;                //offset del siguiente byte libre

    uint32_t capacity;                              ///array capacity
    uint32_t length;                                /// array current usage
    HtModuleEntry *entries;                         /// modules array.
} WpLoadMemory;

uint32_t WpLoadMemoryInit(WpLoadMemory *self, uint8_t *buffer, uint32_t buffer_size, HtModuleEntry *entries, uint32_t capacity);

WpBinFile WpLoadMemoryReserve(WpLoadMemory *self, uint32_t size);

/// @brief Get item from hash table for a given key.
/// @param self 
/// @param key 
/// @param key_len 
/// @return value or NULL if key not found
WpModuleState * HashTableModulesGet(WpLoadMemory *self, const char *key, size_t key_len);

/// @brief Set item with given key to value. If item not exist, a new one is created
/// @param self 
/// @param key 
/// @param value 
/// @return index of the entry if the operation was successful, or 0xFFFFFFFF if it failed
WpModuleState * HashTableModulesSet(WpLoadMemory *self, const char *key, size_t key_len,WpModuleState value);

#ifdef __cplusplus
    }
#endif

#endif
