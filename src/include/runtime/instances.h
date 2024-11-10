/**
 * @file instances.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef WASPC_RUNTIME_INSTANCES_H
#define WASPC_RUNTIME_INSTANCES_H

#ifdef __cplusplus
    extern "C" {
#endif

#include "webassembly/execution/runtime/instances.h"

#include <stdint.h>

typedef struct VectorGlobalInst{
    
    uint32_t len;           //total bytes
    uint8_t *top;         //number to first unused byte, like stack pointer
    uint8_t *global;        //byte array for dense storage
    

} VectorGlobalInst;

void VectorGlobalInstInit(VectorGlobalInst *self, uint32_t len);

uint8_t * VectorGlobalInstWrite(VectorGlobalInst *self, GlobalInst *g);

GlobalInst VectorGlobalInstRead(const uint8_t *idx);

#ifdef __cplusplus
    }
#endif

#endif