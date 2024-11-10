/**
 * @file store.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-05
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef WASPC_RUNTIME_STORE_H
#define WASPC_RUNTIME_STORE_H

#ifdef __cplusplus
    extern "C" {
#endif


#include "runtime/instances.h"

#include <stdint.h>



/**
 * @brief Struct to implemet was's store spec for each module instance
 * 
 */
typedef struct StorePool{

    struct StorePool *next;
    struct StorePool *prev;

    VectorGlobalInst global_inst;

}StorePool;

typedef struct Store
{
    StorePool first;
    
}Store;


#ifdef __cplusplus
    }
#endif

#endif