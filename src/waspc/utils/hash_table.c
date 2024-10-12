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

#include <stdio.h> //TODO remove

/**
 * @brief 
 * 
 * @param slots 
 * @param capacity 
 * @param key 
 * @return HashTableEntry* 
 *
static Slot* FindSlot(Slot *slots, uint32_t capacity, uint32_t key) {

    Slot *slot;
    uint32_t index = key % capacity;
    uint32_t i = 0;
    
    while(index < capacity) {

        slot = &slots[index];
        if (slot->key == 0 || slot->key == key) {
            return slot;
        }
        
        index++;
    }

    return NULL;
}*/

/**
 * @brief 
 * 
 * @param self 
 * @param new_capacity 
 *
static void IncreaseCapacity(HashTable *self, uint32_t new_capacity) {

    uint32_t key;
    Slot *new_slots = ALLOCATE(Slot, new_capacity);
    Slot *free_slot;
    
    //TODO check max capacity and new_capacity > old capacity

    //Init new slots to default values
    for (int i = 0; i < new_capacity; i++) {
        new_slots[i].key = 0;
        new_slots[i].value = NULL;
    }
   
    //inserting all no empty slots into the new one
    for (int ii = 0; ii < self->capacity; ii++) {
        key = self->slots[ii].key;
        //If empty slot continue
        if(key == 0){
            continue;
        }
        
        free_slot = FindSlot(new_slots, new_capacity, key);
        free_slot->key = self->slots[ii].key;
        free_slot->value=self->slots[ii].value;
    }
    
    //releasing old reseved memory
    free(self->slots);
    self->slots = new_slots;

}*/


/**
 * @brief 
 * 
 * @param self 
 * @param data_type 
 * @param init_capacity 
 *
void InitHashTable(HashTable *self, uint32_t init_capacity){

    self->capacity = init_capacity;
    self->usage = 0;
    //seld->data_type = data_type;
    self->slots = ALLOCATE(Slot, init_capacity);

    for(int i = 0; i < init_capacity; i++){
        self->slots[i].key = 0;
        self->slots[i].value = NULL;
    }
    
}*/

/*
void FreeHashTable(HashTable *self){

    self->capacity = 0;
    self->usage = 0;
    self->items = NULL;
}

WpError HashTableSet(HashTable *self, uint32_t key, void *value){

    WpError result = CreateError(WP_DIAG_ID_OK, W_DIAG_MOD_LIST_UTILS, 114, 0);       //No error
    
    if (self->usage + 1 > (self->capacity * HASH_TABLE_MAX_LOAD)/100) {
        uint32_t new_capacity = ((self->usage+1)*100)/75;
        IncreaseCapacity(self, new_capacity);
    }
    
    Slot* match_slot = FindSlot(self->slots, self->capacity, key);

    if(match_slot == NULL){
        result.id = WP_DIAG_ID_TODO;
        return result;
    };

    if (match_slot->key == 0) {
        //new key
        self->usage++;
    }

    match_slot->key = key;
    match_slot->value = value;

    return result;
}

void * HashTableGet(HashTable *self, uint32_t key){

    
    if (self->usage == 0) return NULL;

    Slot *match_slot = FindSlot(self->slots, self->capacity, key);

    if (match_slot->key == 0) return NULL;
    
    return match_slot->value;
}

WpError HasTableDelete(HashTable *self, uint32_t key) {

    WpError result = CreateError(WP_DIAG_ID_OK, W_DIAG_MOD_LIST_UTILS, 153, 0);       //No error

    if (self->usage == 0) return result;

    // Find the entry.
    Slot *match_slot = FindSlot(self->slots, self->capacity, key);

    if (match_slot->key == 0) return result;

    // Place a tombstone in the entry.
    match_slot->key = 0;
    match_slot->value = NULL;
    return result;
}*/

