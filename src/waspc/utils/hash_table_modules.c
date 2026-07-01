/**
 * @file hash_table.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-23
 * 
 * @copyright Copyright (c) 2024
 * 
 */

//#include "diagnostic/error.h"
#include "utils/hash_table_modules.h"


#include <string.h>
#include <assert.h> 
//#include <stdio.h>


/**
 * @brief Fowler–Noll–Vo hash function 
 * 
 * @param key Null terminated string.
 * @param len Length of string.
 * @return uint32_t 
 */
static uint32_t fnv(const char *key, size_t len){

    //Constant definition for FNV algoritm
    #define FNV_PRIME_32 16777619
    #define FNV_OFFSET_BASIC 2166136261u

    
    uint32_t hash = FNV_OFFSET_BASIC;    
    
    for(const char *k = key; k < (key + len); k++){
        hash ^= (uint32_t)(unsigned char)(*k);
        hash *= FNV_PRIME_32;
    }

    return hash;
}


/**
 * @brief Inserts or updates an entry in the hash table using linear probing.
 *
 * This static function computes the hash for the given key and attempts to insert the key-value pair
 * into the hash table. If the key already exists, its value is updated. If an empty slot is found,
 * the new entry is inserted. Linear probing is used to resolve collisions.
 * The caller must ensure that the hash table has been initialized and has sufficient capacity
 *
 * @param self Pointer to the hash table.
 * @param key The key to insert or update.
 * @param key_len The length of the key.
 * @param value Pointer to the value to associate with the key.
 * @return uint32_t The index in the table where the entry was inserted or updated.
 */
static uint32_t HashTableModulesSetEntry(HashTableModules *self, const char *key, size_t key_len, WpModuleState value){
    //TODO key_len should be less than 32, otherwise we will have a buffer overflow. We should handle this case properly, either by truncating the key or returning an error.
    if (key_len >= 32) {
        return 0; // Error: key too long
    }
    
    uint32_t hash = fnv(key, key_len);;
    uint32_t index = hash % self->capacity;
    uint32_t i = 0;             //iterator

    // Loop till we find an empty entry.
    while (i < self->capacity) {

        if(self->entries[index].key[0] == '\0'){
            //empty bucket at index. New entry
            strncpy(self->entries[index].key, key, key_len); //copy key to entry
            self->entries[index].key[key_len] = '\0'; // Ensure null termination
            self->entries[index].module = value;                 //copy value
            self->length++;                                     //increment length
            return index;
        }
        else if (strncmp(key, self->entries[index].key, key_len) == 0) {
            // Found key (it already exists), update value.
            self->entries[index].module = value;
            return index;
        }

        // Key wasn't in this slot, move to next (linear probing).
        index++;
        if (index >= self->capacity) {
            // At end of entries array, wrap around.
            index = 0;
        }
    }

    assert(0); // Should never get here.
    return 0;
}


/**
 * @brief Hash Table constructor.
 * 
 * @param self  
 */
void HashTableModulesInit(HashTableModules *self){
    
    self->length = 0; 
    self->capacity = 0;   
    self->entries = NULL;    
}

/**
 * @brief Retrieves the value associated with a given key from the hash table.
 *
 * This function searches for the specified key using linear probing. If the key is found,
 * it returns the associated value. If the key is not found, it returns NULL.
 * The hash table must be initialized before calling this function.
 *
 * @param self Pointer to the hash table.
 * @param key The key to search for.
 * @param key_len The length of the key.
 * @return void* Pointer to the value associated with the key, or NULL if the key is not found.
 */
WpModuleState * HashTableModulesGet(HashTableModules *self, const char *key, size_t key_len){

    //hash table must be initilised first
    assert(self->capacity > 0);
    // key must be valid
    assert(key);    

    uint32_t hash = fnv(key, key_len);
    uint32_t index = hash % self->capacity;  
    uint32_t i = 0;             //iterator 
    
    //Linear probe algoritm
    while(i < self->capacity) {     //used i instead index for start at begining if key is not found between index and final bucket
        
        if(strncmp(key, self->entries[index].key, key_len) == 0){                
                // Found key, return value.
                return &self->entries[index].module;
        }

        index++;
        
        if (index >= self->capacity) {
            // At end of entries array, wrap around.
            index = 0;
        }

        i++;
    }

    //not found
    return NULL;    
}


/**
 * @brief Inserts or updates an entry in the hash table.
 *
 * This function inserts a key-value pair into the hash table. If the key already exists,
 * its value is updated. If the table is full, the function returns NULL and does not insert the entry.
 * The hash table must be initialized and have sufficient capacity before calling this function.
 *
 * @param self Pointer to the hash table.
 * @param key The key to insert or update.
 * @param key_len The length of the key.
 * @param value Pointer to the value to associate with the key.
 * @return WpModuleState* Pointer to the entry if the operation was successful, or NULL if it failed.
 */
WpModuleState * HashTableModulesSet(HashTableModules *self, const char *key, size_t key_len, WpModuleState value){

    //hash table must be initialise first
    assert(self->capacity > 0);
    //key diferent than null
    assert(key);          
    
    uint32_t index;    
    
    //Check for enought capacity   
    if(self->length < self->capacity){
        index = HashTableModulesSetEntry(self, key, key_len, value);
        return &self->entries[index].module;  //return pointer to the entry
    }
    else{
        //table completly full
        return NULL;
    }
        
}




