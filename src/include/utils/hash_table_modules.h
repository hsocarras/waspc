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

#ifndef WASPC_UTILS_HASH_TABLE_MODULES_H
#define WASPC_UTILS_HASH_TABLE_MODULES_H

#ifdef __cplusplus
    extern "C" {
#endif
#include "utils/hash_table.h"
#include "objects/module_state.h"

#include <stdint.h>

#define HASH_TABLE_MODULES_KEY_LEN 32
typedef struct HtModuleEntry {                            ///Entry for hash table    
    char key[HASH_TABLE_MODULES_KEY_LEN];                                       ///string key TODO config max character sisze. For now fixed to 32 characters
    WpModuleState module;                                    ///any kind value
} HtModuleEntry ;


typedef struct HashTableModules{                           
    uint32_t capacity;                              ///array capacity
    uint32_t length;                                /// array current usage
    HtModuleEntry *entries;
} HashTableModules;

/// @brief Constructor for a hash table
/// @param self 
void HashTableModulesInit(HashTableModules *self);


/// @brief Get item from hash table for a given key.
/// @param self 
/// @param key 
/// @param key_len 
/// @return value or NULL if key not found
WpModuleState * HashTableModulesGet(HashTableModules *self, const char *key, size_t key_len);

/// @brief Set item with given key to value. If item not exist, a new one is created
/// @param self 
/// @param key 
/// @param value 
/// @return index of the entry if the operation was successful, or 0xFFFFFFFF if it failed
WpModuleState * HashTableModulesSet(HashTableModules *self, const char *key, size_t key_len,WpModuleState value);


#ifdef __cplusplus
    }
#endif

#endif