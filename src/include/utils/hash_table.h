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

#ifndef WASPC_UTILS_HASH_TABLE_H
#define WASPC_UTILS_HASH_TABLE_H

#ifdef __cplusplus
    extern "C" {
#endif

#include "webassembly/structure/types.h"

#include <stdint.h>


typedef struct HtEntry {                            ///Entry for hash table    
    Name key;                                       ///string key
    void *value;                                    ///any kind value
} HtEntry ;


typedef struct HashTable{                           ///Implemented as dinamic array of entry
    uint32_t capacity;                              ///array capacity
    uint32_t length;                                /// array current usage
    HtEntry *entries;
} HashTable;

/// @brief Constructor for a hash table
/// @param self 
void HashTableInit(HashTable *self);

void HastTableSetup(HashTable *self, HtEntry *table, uint32_t number_entries);

/// @brief Destructor for a hash table struct
/// @param self 
void HasTableDestroy(HashTable *self);

/// @brief Get item from hash table for a given key.
/// @param self 
/// @param key 
/// @return value or NULL if key not found
void * HashTableGet(HashTable *self, Name key);

/// @brief Set item with given key to value. If item not exist, a new one is created
/// @param self 
/// @param key 
/// @param value 
/// @return value if success, otherwise NULL
HtEntry * HashTableSet(HashTable *self, const Name key, void *value);


#ifdef __cplusplus
    }
#endif

#endif