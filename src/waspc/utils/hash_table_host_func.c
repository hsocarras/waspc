#include "utils/hash_table_host_func.h"

#include <assert.h>

/**
 * @brief Hash Table constructor.
 * 
 * @param self  
 */
void HashTableHostFuncInit(HashTableHostFunc *self){
    
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
 * @return WpBuildinFunction* Pointer to the value associated with the key, or NULL if the key is not found.
 */
const WpBuildinFunction * HashTableHostFuncGet(HashTableHostFunc *self, const char *key, size_t key_len){

    //hash table must be initilised first
    assert(self->capacity > 0);
    // key must be valid
    assert(key);   
    if(key_len > HASH_TABLE_HOST_FUNC_KEY_LEN) {
        // Key length exceeds maximum allowed length
        return NULL;
    } 

    uint32_t hash = fnv(key, key_len);
    uint32_t index = hash % self->capacity;  
    uint32_t i = 0;             //iterator 
    
    //Linear probe algoritm
    while(i < self->capacity) {     //used i instead index for start at begining if key is not found between index and final bucket
        
        if(strncmp(key, self->entries[index].key, key_len) == 0){                
                // Found key, return value.
                return &self->entries[index].func;
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
