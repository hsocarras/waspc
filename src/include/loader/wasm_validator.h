/**
 * @file wasm_validator.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-22
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef WASPC_WASM_VALIDATOR_H
#define WASPC_WASM_VALIDATOR_H

#ifdef __cplusplus
    extern "C" {
#endif

//wasp includes
#include "object/result.h"
#include "object/wasm_module.h"
#include "webassembly/structure/module.h"
#include "webassembly/structure/types.h"

#include <stdint.h>

typedef struct Context{

    // type  
    uint32_t type_len;    
    

} Context;




#ifdef __cplusplus
    }
#endif

#endif