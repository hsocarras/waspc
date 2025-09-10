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

    // types  
    uint32_t functypes_count;               /// number of function types in the module
    const uint8_t *types;                         /// pointer to the types section
    
    //locals
    VecValType locals;
    
    VecValType return_types;  /// return types of the function

} Context;

typedef struct ValCtrlFrame{
    const uint8_t *ip;                    /// pointer to the opcode used whren return from call other frame
    ValType *start_types;               /// pointer to the start types in the value stack (means slot 0 for local get)
    uint32_t start_types_len;           /// length of the start types
    ValType *end_types;                 /// pointer to the end types in the value stack (return types)
    uint32_t end_types_len;             /// length of the end types    
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

/**
 * @brief Function to validate binaries modules.
 * This function iterates throug binary file one section at time, checks the order and validate it.
 */
const uint8_t* ValidateBinSectionById(WpValidatorState *self, const uint8_t *index, const uint8_t section_id, uint8_t *previous_secction, WpModuleState *mod);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

uint8_t WpValPushValType(WpValidatorState *self, ValType val_type);

ValType WpValPopExpectedValType(WpValidatorState *self, ValType val_type);

ValCtrlFrame WpValPopCtrlFrame(WpValidatorState *self);

uint8_t WpValPushCtrlFrame(WpValidatorState *self, ValCtrlFrame frame);

uint32_t WpValEvalOpcode(WpValidatorState *self, OpCode opcode);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Function to validate magic number of a wasm binary file.
 */
uint8_t ValidateMagicBuf(const uint8_t *buf);

/**
 * @brief Function to validate wasm binary version number
 */
uint8_t ValidateVersionBuf(const uint8_t *buf, uint32_t *version_number);

uint8_t ValidateLimitsTypeBuf(const uint8_t *buf, uint32_t k);

uint8_t ValidateTableTypeBuf(const uint8_t *buf);

uint8_t ValidateGlobalTypeBuf(const uint8_t *buf);

uint8_t ValidateValType(uint8_t val_type);

uint8_t ValidateFuncTypeBuf(const uint8_t *buf);

uint8_t ValidateImportBuf(const uint8_t *buf, uint32_t functiontype_count);

uint8_t ValidateExportBuf(const uint8_t *buf, uint32_t functiontype_count, uint32_t table_count, uint32_t memory_count, uint32_t global_count);

uint8_t ValidateElementBuf(const uint8_t *buf, uint32_t function_count, uint32_t table_count);

uint8_t ValidateConstantExprBuf(const uint8_t *buf, uint32_t max_len);

uint8_t ValidateCodeBuf(const uint8_t *buf);

uint8_t ValidateDataBuf(const uint8_t *buf, uint32_t memory_count);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
    }
#endif

#endif