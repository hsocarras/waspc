/**
 * @file values.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-04
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef WASPC_WEBASSEMBLY_EXECUTION_RUNTIME_VALUES_H
#define WASPC_WEBASSEMBLY_EXECUTION_RUNTIME_VALUES_H

#ifdef __cplusplus
    extern "C" {
#endif

#include "webassembly/structure/types.h"

#include <stdint.h>




typedef struct ExternalValue {
    ExternalType type;  
    /*union address
    {
        //FuncInst *func;
        //TableInst *table;
        //MemoryInst *memory;
        //GlobalInst *global;
    };*/
      
}ExternalValue;

typedef struct  VecExternalValue {
    uint32_t lenght;
    ExternalValue elements;
}VecExtrenalValue;


/**
 * @brief 
 * 
 */
typedef struct Trap{
    uint32_t t;
}Trap;





#ifdef __cplusplus
    }
#endif

#endif