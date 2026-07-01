/**
 * @file memory.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-02-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef WASPC_OBJECTS_MEMORY_H
#define WASPC_OBJECTS_MEMORY_H

#ifdef __cplusplus
    extern "C" {
#endif

//wasp includes
#include "objects/object.h"

#include <stdint.h>

#define WAS_PAGE_SIZE 65536

typedef struct WpMemoryInstance {
    /// head for all Waspc object to allow cast
    WpObjectType wp_type;
    struct WpMemoryInstance *next;

    uint8_t imported;                       //flag to indicate if the memory instance is imported or defined in the module.
    struct WpMemoryInstance *address;       //pointer to the memory instance, assigned during module instantiation

    const uint8_t *mem_type;             //pointer to the memory type in the binary file, used for type checking in imports and exports
    uint8_t address_type;            //32 or 64
    uint64_t page_size_min;
    uint64_t page_size_max;
    uint64_t usage;                 //bytes usados.
    uint8_t *bytes;                 //pointer to the memory bytes, allocated statically..
    
    

}WpMemoryInstance;

#define WpMemoryStoreF64(self, offset, data) WpMemInstanceStore(self, offset, (uint8_t *)&data, 8)
#define WpMemoryStoreF32(self, offset, data) WpMemInstanceStore(self, offset, (uint8_t *)&data, 4)
#define WpMemoryStoreI64(self, offset, data) WpMemInstanceStore(self, offset, (uint8_t *)&data, 8)
#define WpMemoryStoreI32(self, offset, data) WpMemInstanceStore(self, offset, (uint8_t *)&data, 4)

// Methods **************************************************************************************************
void WpMemoryInstanceInit(WpMemoryInstance *self);

uint32_t WpMemInstanceStore(WpMemoryInstance *self, uint64_t offset, uint8_t *data, uint32_t data_len);


#ifdef __cplusplus
    }
#endif

#endif