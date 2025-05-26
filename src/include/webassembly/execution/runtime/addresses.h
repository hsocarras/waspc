/**
 * @file addresses.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef WASPC_WEBASSEMBLY_EXECUTION_RUNTIME_ADDRESSES_H
#define WASPC_WEBASSEMBLY_EXECUTION_RUNTIME_ADDRESSES_H
 
#ifdef __cplusplus
    extern "C" {
#endif
 
#include "webassembly/structure/module.h"
 
#include <stdint.h>
 
typedef Func * FuncAddr;
typedef struct VecFuncAddr{
    uint32_t lenght;
    FuncAddr *elements;
}VecFuncAddr;

typedef Table * TableAddr;
typedef struct VecTableAddr{
    uint32_t lenght;
    TableAddr *elements;
}VecTableAddr;

typedef MemType * memaddr;
typedef struct VecMemAddr{
    uint32_t lenght;
    memaddr *elements;
}VecMemAddr;


typedef Global * globaladdr;
typedef struct VecGlobalAddr{
    uint32_t lenght;
    globaladdr *elements;
}VecGlobalAddr;

typedef Elem * elemaddr;
typedef struct VecElemAddr{
    uint32_t lenght;
    elemaddr *elements;
}VecElemAddr;

typedef Data * dataaddr;
typedef struct VecDataAddr{
    uint32_t lenght;
    dataaddr *elements;
}VecDataAddr;

//TODO External address


#ifdef __cplusplus
    }
#endif
 
#endif