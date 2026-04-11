
#include "objects/memory.h"

#include <string.h>


void WpMemoryInstanceInit(WpMemoryInstance *self){
    self->wp_type = WP_OBJECT_MEMORY_INSTANCE;
    self->addres_type = 0;
    self->page_size_min = 0;
    self->page_size_max = 0;
    self->usage = 0;
    self->bytes = NULL;
    self->next = NULL;
}


uint32_t WpMemInstanceStore(WpMemoryInstance *self, uint64_t offset, uint8_t *data, uint32_t data_len){
    if(!self->bytes ){      //TODO|| offset + data_len > self->usage){
        return 1;       //TODO better error handling, maybe return an error object instead of an error code TRAP
    }
    memcpy(self->bytes + offset, data, data_len);
    return 0;
}