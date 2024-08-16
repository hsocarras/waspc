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

typedef struct Entry{
    char *key;
    uint32_t len;
    uint32_t value;
}Entry;

typedef struct HashTable{

    uint32_t capacity;
    uint32_t usage;
    Entry * items;
} HashTable;


void InitHashTable(HashTable *self);
void FreeHashTable(HashTable *self);

#ifdef __cplusplus
    }
#endif

#endif