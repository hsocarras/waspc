/**
 * @file memory.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-06
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "memory/memory.h"

void * Reallocate (void* pointer, size_t oldSize, size_t newSize){

    if (newSize < 0) {
        free(pointer);
        return NULL;
    }
    
    void* result = realloc(pointer, newSize);
    return result;

}