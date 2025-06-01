/**
 * @file runtime.h
 * @author Hector E. Socarras (hsocarras1987@gmail.com)
 * @brief Header file for waspc runtime.
 * @version 0.1
 * @date 2024-06-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef WASPC_RUNTIME_RUNTIME_H
#define WASPC_RUNTIME_RUNTIME_H

#ifdef __cplusplus
    extern "C" {
#endif

//wasp includes
#include "objects/module.h"
#include "objects/error.h"
#include "utils/hash_table.h"
#include "validation/wasm_validator.h"

#include <stdint.h>


/**
 * @brief Sandbox for webasembly runtime, customised for plc.
 * 
 */
typedef struct WpRuntimeState{

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    //uint8_t *in;                            ///pointer to static allocation for input area
    //uint32_t input_size;
    //uint8_t *out;                           ///pointer to static allocation for output area
    //uint32_t output_size;
    //uint8_t *mark;                          ///pointer to static allocation for mark area
    //uint32_t mark_size;    

    /// @brief Properties related to memory to store in ram the web asembly wasm file
    const uint8_t *code_mem_start;
    const uint8_t *code_mem_ptr;
    uint32_t code_mem_size;

    WpError err;                           ///Error object for runtime
    
    /// @brief Hash table for store loaded modules. Online project.
    HashTable modules; 

    WpValidatorState validator;            ///Validator state object
   
    /**
     * @brief TODO This is for mannage the running user programs pou.
     * It should be inplemented later in a hash table or linked list to keep track 
     * for pou's source code loaded in Load Memory
     * 
     */
    //WasmLoadInfo wasm_loaded[2];              

}WpRuntimeState;

/**
 * @brief Runtime constructor.
 * 
 * @param self 
 *  
 */
void WpRuntimeInit(WpRuntimeState *self);

/**
 * @brief Function to init code memory area
 * 
 * @param self 
 * @param start pointer to where memory start
 * @param mem_size memory size in bytes
 */
void WpRuntimeCodeMemInit(WpRuntimeState *self, const uint8_t *start, uint32_t mem_size);

void WpRuntimetableInit(WpRuntimeState *self, HtEntry *table, uint32_t number_entries);

WpObject * WpRuntimeReadModule(WpRuntimeState *self, Name mod_name, const uint8_t *mod_start, uint32_t mod_size);

WpObject * WpRuntimeValidateModule(WpRuntimeState *self, WpModuleState *mod);

WpObject * WpRuntimeInvocateProgram(WpRuntimeState *self, WpModuleInstance *m_instance);

WpObject * WpRuntimeInstanciateModule(WpRuntimeState *self, WpModuleState *mod, ExternalValue *externv, uint32_t extern_len);

#ifdef __cplusplus
    }
#endif

#endif