/**
 * @file load_memory.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */

//Wasp includes
#include "memory/memory.h"
#include "memory/load_memory.h"
#include "diagnostic/error.h"

//standard includes
#include <string.h>


void InitLoadMemoryManager(LoadMemoryManager *self, uint8_t *buf, const uint32_t size){

    self->bank.buf = buf;
    self->bank.size = size;
    self->index = buf;
    self->block_count = 0;

}

uint32_t GetFreeLoadMemory(const LoadMemoryManager *self){

    uint8_t *mem_end = self->bank.buf + self->bank.size;                //pointer to memory end
    return mem_end - self->index;
}

float GetLoadMemoryUsage(const LoadMemoryManager *self){

    
    float mem_used = (float)(self->index - self->bank.buf);  
    float total_mem = (float)(self->bank.size);
    
    return (mem_used/total_mem)*100.0;              
}

WpError AppendBufferToLoadMem(LoadMemoryManager *self, const uint8_t *buf, const uint32_t size){

    WpError result = {OK};                                              //No error
    uint8_t *mem_end = self->bank.buf + self->bank.size;                //pointer to memory end

    //Check enough available space
    if((mem_end - self->index) < size){
        result.id = 3;                                                  //TODO improve later
    }

    memcpy(self->index, buf, size);
    self->index = self->index + size;

    return result;

}

