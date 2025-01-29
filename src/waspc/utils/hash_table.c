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
#include "utils/hash_table.h"
#include "memory/memory.h"

#include <string.h>
#include <assert.h> 
//#include <stdio.h>

/**
 * @brief Fowler–Noll–Vo hash function 
 * 
 * @param key Null terminated string.
 * @return uint32_t 
 */
static uint32_t fnv(const char *key){

    //Constant definition for FNV algoritm
    #define FNV_PRIME_32 16777619
    #define FNV_OFFSET_BASIC 2166136261u

    
    uint32_t hash = FNV_OFFSET_BASIC;    
    
    for(const char *k = key; *k; k++){
        hash ^= (uint32_t)(unsigned char)(*k);
        hash *= FNV_PRIME_32;
    }

    return hash;
}


/**
 * @brief 
 * 
 * @param self 
 * @param new_capacity 
 */
static uint8_t HashTableGrow(HashTable *self, uint32_t new_capacity) {

    //new array
    HtEntry *old_entries = self->entries;
    self->entries = GROW_ARRAY(HtEntry, self->entries, new_capacity);
    
    if(self->entries){
        //Init new slots to default values
        for (int i = self->length; i < new_capacity; i++) {
            self->entries[i].key = NULL;
            self->entries[i].value = NULL;
        }
        self->capacity = new_capacity;
        return 1;
    }
    else{
        self->entries = old_entries;
        return 0;
    }

}

static uint32_t HashTableSetEntry(HashTable *self, const char *key, void *value){

    
    uint32_t hash = fnv(key);
    uint32_t index = hash % self->capacity;
    uint32_t i = 0;             //iterator

    // Loop till we find an empty entry.
    while (i < self->capacity) {

        if(self->entries[index].key == NULL){
            //empty bucket at index. New entry
            self->entries[index].key = key;                     //copy key
            self->entries[index].value = value;                 //copy value
            self->length++;                                     //increment length
            return index;
        }
        else if (strcmp(key, self->entries[index].key) == 0) {
            // Found key (it already exists), update value.
            self->entries[index].value = value;
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
 * @param data_type 
 * @param init_capacity 
 */
void HashTableInit(HashTable *self){
    
    self->length = 0;
    
    self->entries = ALLOCATE(HtEntry, HASH_TABLE_GROW_STEP);

    if (self->entries == NULL) {        
        self->capacity = 0;
    }
    else{        
        self->capacity = HASH_TABLE_GROW_STEP;        
        //Initialize entries
        for(int i = 0; i < HASH_TABLE_GROW_STEP; i++){
            self->entries[i].key = NULL;
            self->entries[i].value = NULL;
        }
    }
    
}

/**
 * @brief Destroy all data in table, not table itself
 * 
 * @param self 
 */
void HashTableDestroy(HashTable *self){

    // First free allocated keys.
    for (size_t i = 0; i < self->capacity; i++) {
        free((void*)self->entries[i].key);
    }

    free(self->entries);
    self->capacity = 0;
    self->length = 0;
    
}

/**
 * @brief 
 * 
 * @param self 
 * @param key 
 * @return void* 
 */
void * HashTableGet(HashTable *self, const char *key){

    //hash table must be initilised first
    assert(self->capacity > 0);
    // key must be valid
    assert(key);

    uint32_t hash = fnv(key);
    uint32_t index = hash % self->capacity;  
    uint32_t i = 0;             //iterator 
    
    //Linear probe algoritm
    while(i < self->capacity) {     //used i instead index for start at begining if key is not found between index and final bucket
        
        if(self->entries[index].key == NULL){
            //empty bucket at index. Key not found
            goto next;
        }
        

        if(strcmp(key, self->entries[index].key) == 0){                
                // Found key, return value.
                return self->entries[index].value;
        }
        
        next:
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
 * @brief Function to set entry into table
 * 
 * @param self 
 * @param key 
 * @param value 
 * @return void* vaule if succes otherwise null
 */
void * HashTableSet(HashTable *self, const char *key, void *value){

    //hash table must be initialise first
    assert(self->capacity > 0);
    //key diferent than null
    assert(key);    
    uint32_t hash = fnv(key);
    uint32_t index;    
    
    //Check for enought capacity
    if (self->length + 1 > (self->capacity * HASH_TABLE_MAX_LOAD)/100) {
        
        //check for max capacity allowed
        if(self->capacity < HASH_TABLE_MAX_CAPACITY){
            uint32_t new_capacity = self->capacity + HASH_TABLE_GROW_STEP;
            //if table grow array success 
            if(HashTableGrow(self, new_capacity)){
                index = HashTableSetEntry(self, key, value);
                return value;
            }
            else{
                return NULL;
            }
        }
        //if table can't grow more
        else{
            //check if are free entry
            if(self->length < self->capacity){
                index = HashTableSetEntry(self, key, value);
                return value;
            }
            else{
                //table completly full
                return NULL;
            }
        }
    }
    else{ //enought space        
        index = HashTableSetEntry(self, key, value);        
        return value;
    }
}




