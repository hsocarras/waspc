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
#include "objects/wp_objects.h"
#include "memory/store.h"
#include "utils/hash_table_modules.h"
#include "decoder/wasm_decoder.h"
#include "interpreter/interpreter.h"
#include "validator/wasm_validator.h"


#include <stdint.h>

/**
 * @brief Structure to represent a WebAssembly binary file in memory.
 */
typedef struct WpBinFile{
    const uint8_t *buf;
    uint32_t bufsize;
} WpBinFile;

/**
 * @brief Sandbox for webasembly runtime.
 * 
 */
typedef struct WpRuntimeState{

    ////////////////////////////////////////////////////////////////////////////////////////////////////    
    //uint32_t input_size;   
    //uint32_t output_size;    
    //uint32_t mark_size;    

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    WpError err;                                ///Error object for runtime
    WpResult result;                            ///Result object for runtime

    /// Memories  ///////////////////////////////////////////////////////////////////////////////////////////////
    WpStore store;                              /// Store for instances according to webassembly specification.

    HashTableModules modules;                   /// Hash table for store module states with their name as key, to allow import resolution by name.

    uint8_t *data_memory;                       /// Pointer to the data memory for memory instaces data.
    uint32_t data_memory_size;                  /// Size of the data memory for memory instaces data.

    StackValue *value_stack;                    /// Pointer to the value stack for store values during execution.
    uint32_t value_stack_size;                  /// Size of the value stack.
    CallFrame *call_stack;                      /// Pointer to the call stack for store function call frames during execution.
    uint32_t call_stack_size;                   /// Size of the call stack.
    uint8_t *block_stack;                       /// Pointer to the block stack for store block information during execution.
    uint32_t block_stack_size;                  /// Size of the block stack.


    ////////////////////////////////////////////////////////////////////////////////////////////////////////////
    

    /// @brief Interpreter state object for run webassembly code
    WpInterpreterState interpreter;             /// Interpreter state object 

    /// @brief Validator state object for validate webassembly modules
    WpValidatorState validator;                 /// Validator state object
   
    
                

}WpRuntimeState;

/**
 * @brief Runtime constructor.
 * 
 * @param self 
 *  
 */
void WpRuntimeInit(WpRuntimeState *self);

uint32_t WpRuntimeSetMemoryStore(WpRuntimeState *self, uint8_t *mem, uint32_t mem_size);

uint32_t WpRuntimeSetMemoryHashTable(WpRuntimeState *self, HtModuleEntry *mods, uint32_t entries);

uint32_t WpRuntimeSetMemoryData(WpRuntimeState *self, uint8_t *data, uint32_t data_size);

uint32_t WpRuntimeSetMemoryValueStack(WpRuntimeState *self, StackValue *stack, uint32_t stack_size);

uint32_t WpRuntimeSetMemoryCallStack(WpRuntimeState *self, CallFrame *stack, uint32_t stack_size);

uint32_t WpRuntimeSetMemoryBlockStack(WpRuntimeState *self, uint8_t *stack, uint32_t stack_size);

/**
 * @brief Creates a module state from a WebAssembly binary file.
 * 
 * @param mod_state Pointer to a WpModuleState structure to be initialized.
 * @param bin_file A WpBinFile structure containing the binary data and its size.
 * @param mod_name Name to assign to the created module state.
 */
WpObject *WpRuntimeCreateModuleFromBinFile(WpRuntimeState *self, WpBinFile bin_file, const char *mod_name);

/**
 * @brief Validates a WebAssembly module loaded into the runtime.
 *
 * This function checks the integrity and correctness of the module's binary format,
 * including the magic number, version, and the order and validity of all sections.
 * If the module is valid, its status is updated to WP_MODULE_STATUS_VALIDATED.
 * If any validation step fails, an error object is returned and the module status
 * is set to WP_MODULE_STATUS_INVALID.
 *
 * @param self Pointer to the runtime state.
 * @param mod Pointer to the module state to validate.
 * @return WpObject* Returns the validated module object on success,
 *                   or a pointer to an error object on failure.
 */
WpObject * WpRuntimeValidateModule(WpRuntimeState *self, WpModuleState *mod);

/**
 * @brief Instantiates a validated WebAssembly module within the runtime.
 *
 * This function creates a module instance from a validated module, resolving all imports
 * using the provided array of external values. It checks that the module is validated and
 * that the number of provided external values matches the number of imports required.
 * On success, the module is instantiated and ready for execution.
 * On failure, an error object is returned.
 *
 * @param self Pointer to the runtime state.
 * @param mod Pointer to the validated module state to instantiate.
 * @param externv Array of external values to satisfy the module's imports.
 * @param extern_len Number of external values in the array.
 * @return WpObject* Returns the instantiated module object on success,
 *                   or a pointer to an error object on failure.
 */
WpObject * WpRuntimeInstanciateModule(WpRuntimeState *self, WpModuleState *mod);

//WpObject * WpRuntimeInvocateProgram(WpRuntimeState *self, WpModuleInstance *m_instance);

WpObject *WpRuntimeInvokeFunction(WpRuntimeState *self, WpModuleInstance *mod, char *func_name, StackValue *args, uint32_t argc);

#ifdef __cplusplus
    }
#endif

#endif