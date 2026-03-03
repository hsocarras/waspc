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
#include "objects/wp_objects.h"

#include <stdint.h>



typedef struct Context{

    // types  
    uint32_t functypes_count;               /// number of function types in the module
    const uint8_t *types;                         /// pointer to the types section
    
    //locals
    //VecValType locals;
    
    //VecValType return_types;  /// return types of the function

} Context;


typedef struct WpValidatorState{

    WpError err;                                ///Error object for runtime

    Context c;
    Context c_prime;

    //TODO val_satck size for validator must be same size than evaluator
    /// size of the value stack
    //Value *val_stack;               /// stack to store value types
    //Value *stk_ptr;                 /// pointer to the top of the value stack
    uint32_t val_stack_size;        /// size of the value stack

    //ActivationFrame *ctrl_stack;            /// control stack to store control frames
    int32_t ctr_stack_idx;                  /// index to the top of the control stack
    uint32_t ctr_stack_size;                /// size of the control stack

    uint8_t *ip;                /// instruction pointer

}WpValidatorState;

/// Methods (Functions with self) ///////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Function to initialize the validator state.
 */
void WpValidatorStateInit(WpValidatorState *self);

/// Main validation function 
/**
 * @brief Function to validate binaries modules.
 * This function iterates throug binary file one section at time, checks the order and validate it.
 */
const uint8_t* ValidateBinSectionById(WpValidatorState *self, const uint8_t *index, const uint8_t section_id, uint8_t *previous_secction, WpModuleState *mod);

//uint8_t WpValPushValType(WpValidatorState *self, ValType val_type);

//ValType WpValPopExpectedValType(WpValidatorState *self, ValType val_type);

//ValCtrlFrame WpValPopCtrlFrame(WpValidatorState *self);

//uint8_t WpValPushCtrlFrame(WpValidatorState *self, ValCtrlFrame frame);

//uint32_t WpValEvalOpcode(WpValidatorState *self, OpCode opcode);

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Auxiliary functions for validation of sections.
/// They are called by the main validation function when validating each section.
/// They are implemented in the wasm_validator_buffer.c file and are used to validate the binary syntax of each section's content.

uint8_t ValidateTypeBuf(const uint8_t *buf, uint32_t type_count);

uint8_t ValidateImportBuf(const uint8_t *buf, uint32_t functiontype_count);

uint8_t ValidateTableTypeBuf(const uint8_t *buf);

uint8_t ValidateLimitsTypeBuf(const uint8_t *buf, uint32_t k);

uint8_t ValidateMemTypeBuf(const uint8_t *buf);

uint8_t ValidateTagTypeBuf(const uint8_t *buf);

uint8_t ValidateGlobalTypeBuf(const uint8_t *buf);

uint8_t ValidateExportBuf(const uint8_t *buf, uint32_t functiontype_count, uint32_t table_count, uint32_t memory_count, uint32_t global_count);

uint8_t ValidateElementBuf(const uint8_t *buf, uint32_t function_count, uint32_t table_count);

uint8_t ValidateCodeBuf(const uint8_t *buf);

uint8_t ValidateDataBuf(const uint8_t *buf, uint32_t memory_count);

////// Validatiotn for Magic and Version /////////////////////////////////////////////

/**
 * @brief Function to validate magic number of a wasm binary file.
 */
uint8_t ValidateMagicBuf(const uint8_t *buf);

/**
 * @brief Function to validate wasm binary version number
 */
uint8_t ValidateVersionBuf(const uint8_t *buf, uint32_t *version_number);

uint8_t ValidateFuncTypeBuf(const uint8_t *buf);

uint8_t ValidateConstantExprBuf(const uint8_t *buf, uint32_t max_len);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




#ifdef __cplusplus
    }
#endif

#endif