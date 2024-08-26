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

#include <stdint.h>

#define HASH_TABLE_MAX_LOAD 85

#define HASH_TABLE_MAX_CAPACITY 100 //TODO come from config



typedef struct HashTableSlot {     
    uint32_t key;                               /// complete hash
    void *value;
} Slot ;

typedef struct HashTable{    
    uint32_t capacity;
    uint32_t usage;    
    Slot *slots;
} HashTable;


void InitHashTable(HashTable *self, uint32_t init_capacity);
//void FreeHashTable(HashTable *self);
WpError HashTableSet(HashTable *self, uint32_t key, void *value);
void * HashTableGet(HashTable *self, uint32_t key);
WpError HasTableDelete(HashTable *self, uint32_t key);


uint32_t fnv(const char *key, uint32_t len);

#ifdef __cplusplus
    }
#endif

#endif