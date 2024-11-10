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
#include "memory/work_memory.h"
#include "diagnostic/diag.h"

//standard includes
//#include <stdio.h>
#include <string.h>

void InitCodeMem(WorkMemory *self, uint8_t * buf, uint32_t size){

    self->code_size = size;
    self->code = buf;    
    self->index = buf;
}

void InitWorkMemory(WorkMemory *self){

    self->code_size = 0;
    self->code = NULL;    
    self->index = NULL;
    self->block_count = 0;

}


uint32_t GetFreeCodeMem(const WorkMemory *self){

    uint8_t *mem_end = self->code + self->code_size;                //pointer to memory end
    return mem_end - self->index;
}

/*
float GetLoadMemoryUsage(const WorkMemory *self){

    
    float mem_used = (float)(self->index - self->bank.buf);  
    float total_mem = (float)(self->bank.size);
    
    return (mem_used/total_mem)*100.0;              
}*/
/*
WpError AppendWasmCode(WorkMemory *self, const uint8_t *buf, const uint32_t size){

    WpError result = CreateError(OK);                                              //No error
    uint8_t *mem_end = self->code + self->code_size;                //pointer to memory end
   
    //Check enough available space
    if((mem_end - self->index) < size){

        result.id = 3;                                                  //TODO improve later        
        return result;
    }

    memcpy(self->index, buf, size);
    self->index = self->index + size;
    
    return result;

}*/

