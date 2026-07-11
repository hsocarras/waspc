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

#ifndef WASPC_UTILS_HASH_TABLE_HOST_FUNC_H
#define WASPC_UTILS_HASH_TABLE_HOST_FUNC_H

#ifdef __cplusplus
    extern "C" {
#endif

#include "utils/hash_table.h"
#include "objects/function.h"

#include <stdint.h>

#define HASH_TABLE_HOST_FUNC_KEY_LEN 16



typedef struct HtHostFuncEntry {                    ///Entry for hash table    
    char key[HASH_TABLE_HOST_FUNC_KEY_LEN];                                       
    WpBuildinFunction func;                         ///entry content
} HtHostFuncEntry;


typedef struct HashTableHostFunc{                           
    uint32_t capacity;                              ///array capacity
    uint32_t length;                                /// array current usage
    const HtHostFuncEntry *entries;
} HashTableHostFunc;

/// Macro for init hash table with default capacity
void HashTableHostFuncInit(HashTableHostFunc *self);

/// @brief Get item from hash table for a given key.
/// @param self 
/// @param key 
/// @param key_len 
/// @return value or NULL if key not found
const WpBuildinFunction * HashTableHostFuncGet(HashTableHostFunc *self, const char *key, size_t key_len);

#ifdef __cplusplus
    }
#endif

#endif