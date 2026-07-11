

#include "memory/load_memory.h"

#include <string.h>
#include <assert.h> 

/**
 * @brief Aligns a byte offset to the next load-memory alignment boundary.
 *
 * This helper is used before reserving a new segment so that every loaded
 * binary starts at an address compatible with WP_LOAD_MEMORY_ALIGMENT.
 * If value is already aligned, it is returned unchanged. If rounding up would
 * overflow uint32_t, UINT32_MAX is returned as an invalid/sentinel offset.
 *
 * @param value Offset in bytes to align.
 * @return Aligned offset, or UINT32_MAX when the aligned value cannot be
 *         represented in uint32_t.
 */
static inline uint32_t WpLoadMemoryAlignUp(uint32_t value)
{
    uint32_t remainder;
    uint32_t padding;
    
    remainder = value % WP_LOAD_MEMORY_ALIGMENT;
    if(remainder == 0u)
    {
        return value;
    }

    padding = WP_LOAD_MEMORY_ALIGMENT - remainder;
    if(value > UINT32_MAX - padding)
    {
        return UINT32_MAX;
    }

    return value + padding;
}

/**
 * @brief Set to 0 all bytes in buffer
 */
static inline void WpLoadMemoryClearBuffer(WpLoadMemory *self)
{
    if(!self || !self->buffer)
    {
        return;
    }

    memset(self->buffer, 0, self->buffer_size);
}

/**
 * @brief Calculate how many bytes are used
 */
static inline uint32_t WpLoadMemoryUsedBytes(const WpLoadMemory *self)
{
    if(!self)
    {
        return 0u;
    }

    return self->cursor;
}

/**
 * @brief Calculate how many bytes are fee.
 */
static inline uint32_t WpLoadMemoryFreeBytes(const WpLoadMemory *self)
{
    if(!self || self->cursor > self->buffer_size)
    {
        return 0u;
    }

    return self->buffer_size - self->cursor;
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
static uint32_t HashTableModulesSetEntry(WpLoadMemory *self, const char *key, size_t key_len, WpModuleState value){
    
    if (key_len >= HASH_TABLE_MODULES_KEY_LEN) {
        return UINT32_MAX;
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
 * @brief Initializes load memory storage and its module hash table.
 *
 * The caller owns both the load-memory buffer and the hash-table entry array.
 * This function stores those pointers, resets the load-memory cursor, clears
 * the provided buffer, and initializes every hash-table entry as empty.
 *
 * @param self Load-memory instance to initialize.
 * @param buffer Backing storage used to keep loaded binary data.
 * @param buffer_size Size of buffer in bytes. Must be greater than zero.
 * @param entries Entry array used by the module hash table.
 * @param capacity Number of entries available in entries. Must be greater than zero.
 * @return WP_LOAD_MEMORY_OK on success, or WP_LOAD_MEMORY_ERR_INVALID_ARGUMENT
 *         when any required pointer is NULL or a size/capacity is zero.
 */
uint32_t WpLoadMemoryInit(WpLoadMemory *self, uint8_t *buffer, uint32_t buffer_size, HtModuleEntry *entries, uint32_t capacity)
{
    if(!self || !buffer || buffer_size == 0u || !entries || capacity == 0u)
    {
        return WP_LOAD_MEMORY_ERR_INVALID_ARGUMENT;
    }
    
    //set buffer
    self->buffer = buffer;
    self->buffer_size = buffer_size;
    self->cursor = 0u;
    WpLoadMemoryClearBuffer(self);

    //set hash table
    self->entries = entries;
    self->capacity = capacity;
    self->length = 0u;

    // Initialize entries
    for (int i = 0; i < capacity; i++)
    {
        entries[i].key[0] = '\0';
        WpModuleStateInit(&entries[i].module);
    }

    return WP_LOAD_MEMORY_OK;
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
WpModuleState * HashTableModulesGet(WpLoadMemory *self, const char *key, size_t key_len){

    //hash table must be initilised first
    assert(self->capacity > 0);
    // key must be valid
    assert(key);    
    if(key_len > HASH_TABLE_MODULES_KEY_LEN) {
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
WpModuleState * HashTableModulesSet(WpLoadMemory *self, const char *key, size_t key_len, WpModuleState value){

     //hash table must be initialise first
    assert(self->capacity > 0);
    //key diferent than null
    assert(key);          
    
    uint32_t index;    
    
    //Check for enought capacity   
    if(self->length < self->capacity){
        index = HashTableModulesSetEntry(self, key, key_len, value);
        if(index == UINT32_MAX){
            return NULL;
        }
        return &self->entries[index].module;  //return pointer to the entry
    }
    else{
        //table completly full
        return NULL;
    }        
}

/**
 * @brief Reserves a contiguous block in load memory and returns it as a binary view.
 *
 * The reservation starts at the next WP_LOAD_MEMORY_ALIGMENT boundary from the
 * current cursor. On success the cursor is advanced by size bytes. On failure,
 * the returned WpBinFile has buf set to NULL and bufsize set to 0.
 *
 * @param self Load memory arena where the binary will be reserved.
 * @param size Number of bytes to reserve.
 * @return WpBinFile pointing to the reserved range, or {NULL, 0} on failure.
 */
WpBinFile WpLoadMemoryReserve(WpLoadMemory *self, uint32_t size)
{
    uint32_t offset;
    WpBinFile bin_file;

    bin_file.buf = NULL;
    bin_file.bufsize = 0u;

    if(!self || !self->buffer || size == 0u)
    {
        return bin_file;
    }

    //get start point for bin file
    offset = WpLoadMemoryAlignUp(self->cursor);
    if(offset == UINT32_MAX || offset > self->buffer_size)
    {
        return bin_file;
    }

    if(size > self->buffer_size - offset)
    {
        return bin_file;
    }

    bin_file.buf = self->buffer + offset;
    bin_file.bufsize = size;
    self->cursor = offset + size;

    return bin_file;
}




