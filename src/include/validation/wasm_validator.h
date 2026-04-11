/**
 * @file wasm_validator.h
 * @author your name (you@domain.com)
 * @brief  Header file with public Api for validation module
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



typedef struct WpValContext{

    // types  
    uint32_t types_count;                           /// number of types in the module
    const uint8_t *types;                           /// pointer to the types section
    
    uint32_t functions;                             /// number of function declared
    
    const uint8_t *tables;
    uint32_t tables_count;

    const uint8_t *memories;
    uint32_t memories_count;

    const uint8_t *globals;
    uint32_t globals_count;

    const uint8_t *tags;
    uint32_t tags_count;

    const uint8_t *elements;
    uint32_t elements_count;

    const uint8_t *datas;
    uint32_t datas_count;

    const uint8_t *locals;
    uint32_t locals_count;

    const uint8_t *labels;
    uint32_t labels_count;

    const uint8_t *return_type;
    
    const uint8_t *references;
    uint32_t references_count;

} WpValContext;


typedef struct WpValidatorState{

    WpModuleState *mod;                         /// pointer to module object
    WpError *err;                                ///Error object for runtime

    //WpValContext c;                            //context
    //WpValContext c_prime;                      //context auxiliary

    //TODO val_satck size for validator must be same size than evaluator
    /// size of the value stack
    StackValue *value_stack;                    /// stack to store value types
    StackValue *value_stack_top;                /// pointer to the top of the value stack
    StackValue *value_stack_end;                /// pointer to the end of the value stack

    //ActivationFrame *ctrl_stack;            /// control stack to store control frames
    //int32_t ctr_stack_idx;                  /// index to the top of the control stack
    //uint32_t ctr_stack_size;                /// size of the control stack

    const uint8_t *ip;                /// instruction pointer

}WpValidatorState;

/// Methods (Functions with self) ///////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief Function to initialize the validator state.
 */
void WpValidatorStateInit(WpValidatorState *self);


/// @brief Main function
/// @param self 
/// @param mod 
/// @return 
WpObject *WpValidatorValidateModule(WpValidatorState *self, WpModuleState *mod);

// Validation function for each section//////////////////////////////////////////////////////////////////////////////////////
uint32_t ValidateGlobal(WpValidatorState *self, const uint8_t *index, const uint8_t *global_section_end);

uint32_t ValidateExport(WpValidatorState *self, const uint8_t *index, const uint8_t *export_section_end);

uint32_t WpValidateConstantExpresion(WpValidatorState *self,StackValType type, const uint8_t *index, const uint8_t *end_index);

#ifdef __cplusplus
    }
#endif

#endif