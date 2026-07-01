
#include "objects/memory.h"

#include <string.h>

/*
 * @brief Default constructor for WpMemoryInstance objects
 * 
 * @param self Pointer to the WpMemoryInstance object to initialize
 */
void WpMemoryInstanceInit(WpMemoryInstance *self){
    self->wp_type = WP_OBJECT_MEMORY_INSTANCE;

    self->imported = 0;
    self->address = NULL;

    self->address_type = 0;
    self->page_size_min = 0;
    self->page_size_max = 0;
    self->usage = 0;
    self->bytes = NULL;

    self->next = NULL;
}

/**
 * @brief Store data in the memory instance at a specified offset. Used for eval instructions n.store.
 * 
 * @param self Pointer to the WpMemoryInstance object
 * @param offset The offset at which to store the data
 * @param data Pointer to the data to store
 * @param data_len The length of the data to store
 * @return 0 on success, 1 on failure

 */
uint32_t WpMemInstanceStore(WpMemoryInstance *self, uint64_t offset, uint8_t *data, uint32_t data_len){
    if(!self->bytes ){      
        return 1;       
    }
    //copy the data to the memory instance's bytes at the specified offset
    memcpy(self->bytes + offset, data, data_len);
    //updating the usage of the memory instance
    if(offset + data_len > self->usage){
        self->usage = offset + data_len;
    }
    
    return 0;
}