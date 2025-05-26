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
#include "webassembly/structure/types.h"

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
static uint32_t fnv(Name key){

    //Constant definition for FNV algoritm
    #define FNV_PRIME_32 16777619
    #define FNV_OFFSET_BASIC 2166136261u

    
    uint32_t hash = FNV_OFFSET_BASIC;    
    
    for(const char *k = key.name; k < (key.name + key.lenght); k++){
        hash ^= (uint32_t)(unsigned char)(*k);
        hash *= FNV_PRIME_32;
    }

    return hash;
}



static uint32_t HashTableSetEntry(HashTable *self, Name key, void *value){

    
    uint32_t hash = fnv(key);
    uint32_t index = hash % self->capacity;
    uint32_t i = 0;             //iterator

    // Loop till we find an empty entry.
    while (i < self->capacity) {

        if(self->entries[index].key.name == NULL){
            //empty bucket at index. New entry
            self->entries[index].key.name = key.name;                     //copy key
            self->entries[index].key.lenght = key.lenght;                 //copy lenght
            self->entries[index].value = value;                 //copy value
            self->length++;                                     //increment length
            return index;
        }
        else if (strncmp(key.name, self->entries[index].key.name, key.lenght) == 0) {
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
 */
void HashTableInit(HashTable *self){
    
    self->length = 0; 
    self->capacity = 0;   
    self->entries = NULL;    
}

/**
 * @brief 
 * 
 * @param self 
 * @param table 
 * @param number_entries 
 */
void HastTableSetup(HashTable *self, HtEntry *table, uint32_t number_entries){

    assert(table);
    assert(number_entries);

    //Initialize entries
    for(int i = 0; i < number_entries; i++){
        table[i].key.lenght = 0;
        table[i].key.name = NULL;
        table[i].value = NULL;
    }

    self->capacity = number_entries;
    self->entries = table;
}


/**
 * @brief 
 * 
 * @param self 
 * @param key 
 * @return void* 
 */
void * HashTableGet(HashTable *self, Name key){

    //hash table must be initilised first
    assert(self->capacity > 0);
    // key must be valid
    assert(key.name);    

    uint32_t hash = fnv(key);
    uint32_t index = hash % self->capacity;  
    uint32_t i = 0;             //iterator 
    
    //Linear probe algoritm
    while(i < self->capacity) {     //used i instead index for start at begining if key is not found between index and final bucket
        
        if(self->entries[index].key.name == NULL){
            //empty bucket at index. Key not found
            goto next;
        }
        

        if(strncmp(key.name, self->entries[index].key.name, key.lenght) == 0){                
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
void * HashTableSet(HashTable *self, Name key, void *value){

    //hash table must be initialise first
    assert(self->capacity > 0);
    //key diferent than null
    assert(key.name);     
       
    uint32_t hash = fnv(key);
    uint32_t index;    
    
    //Check for enought capacity   
    if(self->length < self->capacity){
        index = HashTableSetEntry(self, key, value);
        return value;
    }
    else{
        //table completly full
        return NULL;
    }
        
}




