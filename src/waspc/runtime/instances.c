/**
 * @file init_runtime.c
 * @author Hector E. Socarras (hsocarras1987@gmail.com)
 * @brief  File with funtion's implementation for initialize waspc runtime and setup all necesary things.
 * @version 0.1
 * @date 2024-08-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "runtime/instances.h"
#include "webassembly/execution/runtime/instances.h"
#include "memory/memory.h"

#include <stdint.h>
#include <string.h>



/**
 * @brief 
 * 
 * @param self 
 * @param len 
 */
void VectorGlobalInstInit(VectorGlobalInst *self, uint32_t len){
        
    if(len > 0){
        self->global = ALLOCATE(uint8_t, len);
        if(!self->global){
            self->len = 0;
            self->top = NULL;
        }
        else{
            self->len = len;  
            self->top = self->global;
            for(int i = 0; i < len; i++){
                self->global[i] = WAS_UNDEF;
            }
        }              
    }
    else{
        self->len = 0;
        self->global = NULL;
    }
    
}

/**
 * @brief 
 * 
 * @return WpResult 
 */
uint8_t * VectorGlobalInstWrite(VectorGlobalInst *self, GlobalInst *g){


    uint8_t *addr = NULL;
    uint8_t *start = self->top;
    uint8_t *end = self->global + self->len;
    uint8_t value_size = 0;
    
    switch(g->val_type){

        case WAS_I32:            
            value_size = 4;
            //check capacity available. Because top is pointing to next free byte 
            //end-top return the number of availables bytes minus 1.
            //We need space for value size and mutable property and type so, we add size plus one to top
            //pointer to get the necesary free space.
            if(end - 5 > self->top){                
                //first byte is value type
                *self->top = g->val_type;
                self->top++;
                *self->top = g->mut;
                self->top++;
                memcpy(self->top, &g->value.s32, 4);

            }
            else{
                //Reallocate array
            }
            break;
        case WAS_I64:
            value_size = 8;
            if(end - 9 > self->top){

            }
            break;
        case WAS_F32:
            value_size = 4;
            if(end - 5 > self->top){

            }
            break;
        case WAS_F64:
            value_size = 8;
            if(end - 9 > self->top){

            }
            break;
        case WAS_V128:
            value_size = 16;
            if(end - 17 > self->top){

            }
            break;
        case WAS_FUNC_REF_TYPE:
            break;
        case WAS_EXTERN_REF_TYPE:
            break;
        default:
            break;
    }

    addr = start;
    return addr;

}


GlobalInst VectorGlobalInstRead(const uint8_t *idx ){

    GlobalInst g;

    g.val_type = *idx;
    idx++;
    switch (g.val_type)
    {
    case WAS_I32:
        g.mut = *idx;
        idx++;
        memcpy(&g.value.s32, idx, 4);
        return g;
    
    default:
        break;
    }
    return g;
}