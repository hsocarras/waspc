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
#include "objects/module.h"
#include "webassembly/structure/module.h"
#include "webassembly/structure/types.h"

#include <stdint.h>

typedef struct Context{

    // type  
    uint32_t type_len;    
    

} Context;

typedef struct WpValidatorState{

    Context c;
    Context c_prime;

}WpValidatorState;

void WpValidatorStateInit(WpValidatorState *self);

uint8_t ValidateValType(uint8_t val_type);

/// @brief Funtion to validate module magic number
/// @param buf 
/// @return 
uint8_t ValidateMagic(const uint8_t *buf);

/// @brief Funtion to validate module version number
uint8_t ValidateVersion(const uint8_t *buf, uint32_t *version_number);

const uint8_t* ValidateBinSectionById(const uint8_t *index, const uint8_t section_id, uint8_t *previous_secction, WpModuleState *mod);

#ifdef __cplusplus
    }
#endif

#endif