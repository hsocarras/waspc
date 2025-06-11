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
#include "interpreter/interpreter.h"

#include <stdint.h>


/**
 * @brief Sandbox for webasembly runtime, customised for plc.
 * 
 */
typedef struct WpRuntimeState{

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    //uint8_t *in;                              ///pointer to static allocation for input area
    //uint32_t input_size;
    //uint8_t *out;                             ///pointer to static allocation for output area
    //uint32_t output_size;
    //uint8_t *mark;                            ///pointer to static allocation for mark area
    //uint32_t mark_size;    

    /// @brief Properties related to memory to store in ram the web asembly wasm file
    const uint8_t *code_mem_start;
    uint8_t *code_mem_ptr;
    uint32_t code_mem_size;

    WpError err;                                ///Error object for runtime
    
    /// @brief Hash table for store loaded modules. Online project.
    HashTable modules; 

    WpValidatorState validator;                 /// Validator state object
   
    WpInterpreterState interpreter;             /// Interpreter state object             

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

/**
 * @brief Loads a WebAssembly binary module into the runtime memory.
 *
 * This function copies the provided binary module into the runtime's code memory area,
 * creates and registers a new module state structure, and associates it with the given name.
 * It checks for sufficient memory and proper initialization of the module table.
 * If an error occurs (such as insufficient memory or uninitialized table), an error object is returned.
 *
 * @param self Pointer to the runtime state.
 * @param mod_name Name identifier for the module to be loaded.
 * @param mod_start Pointer to the start of the buffer containing the binary module.
 * @param mod_size Size of the module in bytes.
 * @return WpObject* Returns a pointer to the loaded module object on success,
 *                   or a pointer to an error object on failure.
 */
WpObject * WpRuntimeReadModule(WpRuntimeState *self, Name mod_name, const uint8_t *mod_start, uint32_t mod_size);

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
WpObject * WpRuntimeInstanciateModule(WpRuntimeState *self, WpModuleState *mod, ExternalValue *externv, uint32_t extern_len);

WpObject * WpRuntimeInvocateProgram(WpRuntimeState *self, WpModuleInstance *m_instance);

WpObject * WpFuncInstanceInvoke(WpRuntimeState *self, funcaddr func, Value *args, uint32_t argc);

#ifdef __cplusplus
    }
#endif

#endif