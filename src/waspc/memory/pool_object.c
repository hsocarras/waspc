/**
 * @file pool_object.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-02-15
 * 
 * @copyright Copyright (c) 2025
 * 
 */

 #include "memory/object_pool.h"

 void WpObjectPoolInit(WpObjectPool *self){
    self->pool_size = 0;
    self->pool = NULL;
    self->free_list = NULL;
 }

 void WpObjectPoolSet(WpObjectPool *self, PoolObject *pool, uint32_t size){

    self->pool_size = size;
    self->pool = pool;

    for(uint32_t i = 0; i < size -1; i++){
        self->pool[i].next =  &(self->pool[i+1]);           //get address for next item        
    }

    self->free_list = pool;                                 //init free list for fast allocation
    
    //finishing init las object
    self->pool[size-1].next = NULL;
 }

 PoolItem * BorrowItem(WpObjectPool *self){

    if(self->free_list){
        PoolObject *new_func = self->free_list;
        self->free_list = new_func->next;                
        return &(new_func->obj);
    }

    return NULL;
 }

 void ReturnItem(WpObjectPool *self, PoolItem *obj){
    //get index 
    uint32_t i = ((uintptr_t)obj - (uintptr_t)self->pool)/sizeof(PoolObject);

    assert(&(self->pool[i].obj) == obj);

    PoolObject *po = &(self->pool[i]);
    po->next = self->free_list;
    self->free_list = po;

 }