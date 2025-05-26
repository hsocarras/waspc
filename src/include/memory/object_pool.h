/**
 * @file 
 * @author Hector E. Socarras (hsocarras1987@gmail.com)
 * @brief Header file for wasm load module
 * @version 0.1
 * @date 2024-06-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef WASPC_MEMORY_OBJECT_POOL_H
#define WASPC_MEMORY_OBJECT_POOL_H

#ifdef __cplusplus
    extern "C" {
#endif

//wasp includes
#include "objects/function.h"

#include <stdint.h>

typedef union PoolItem{
    WpFunctionInstance func;
}PoolItem;

typedef struct PoolObject{

    PoolObject *next;    
    PoolItem obj;

}PoolObject;

typedef struct WpObjectPool {

    uint32_t pool_size;
    PoolObject *pool;
    PoolObject *free_list;    

}WpObjectPool;

//methods
void WpObjectPoolInit(WpObjectPool *self);

void WpObjectPoolSet(WpObjectPool *self, PoolObject *pool, uint32_t size);

PoolItem * BorrowItem(WpObjectPool *self);

void ReturnItem(WpObjectPool *self, PoolItem *obj);

#ifdef __cplusplus
    }
#endif

#endif