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
#include "webassembly/binary/instructions.h"
#include <stdint.h>

typedef struct Context{

    // type  
    uint32_t type_len;    
    

} Context;

typedef struct ValCtrlFrame{
    OpCode op;                      
    ValType *start_types;
    uint32_t start_types_len;           /// length of the start types
    ValType *end_types;
    uint32_t end_types_len;             /// length of the end types
    ValType *val_stack;                /// pointer to the value stack
    uint8_t unreachable;               /// unreachable flag
}ValCtrlFrame;

typedef struct WpValidatorState{

    Context c;
    Context c_prime;

    //TODO val_satck size for valodator must be same size than evaluator
     /// size of the value stack
    ValType *val_stack;  /// stack to store value types
    ValType *stk_ptr; /// pointer to the top of the value stack
    uint32_t val_stack_size; /// size of the value stack

    ValCtrlFrame *ctrl_stack;                /// control stack to store control frames
    int32_t ctr_stack_idx;              /// index to the top of the control stack
    uint32_t ctr_stack_size;           /// size of the control stack

    uint8_t *ip;                /// instruction pointer

}WpValidatorState;

/**
 * @brief Function to initialize the validator state.
 */
void WpValidatorStateInit(WpValidatorState *self);

uint8_t ValidateValType(uint8_t val_type);

/**
 * @brief Function to validate magic number of a wasm binary file.
 */
uint8_t ValidateMagic(const uint8_t *buf);

/**
 * @brief Function to validate wasm binary version number
 */
uint8_t ValidateVersion(const uint8_t *buf, uint32_t *version_number);

/**
 * @brief Function to validate binaries modules.
 * This function iterates throug binary file one section at time, checks the order and validate it.
 */
const uint8_t* ValidateBinSectionById(WpValidatorState *self, const uint8_t *index, const uint8_t section_id, uint8_t *previous_secction, WpModuleState *mod);

uint8_t WpValPushValType(WpValidatorState *self, ValType val_type);

ValType WpValPopExpectedValType(WpValidatorState *self, ValType val_type);

ValCtrlFrame WpValPopCtrlFrame(WpValidatorState *self);

uint8_t WpValPushCtrlFrame(WpValidatorState *self, ValCtrlFrame frame);

uint32_t WpValEvalOpcode(WpValidatorState *self, OpCode opcode);

#ifdef __cplusplus
    }
#endif

#endif