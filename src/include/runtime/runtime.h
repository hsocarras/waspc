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
#include "webassembly/binary/module.h"
#include "webassembly/runtime/module_instance.h"
#include "diagnostic/error.h"
//#include "memory/work_memory.h"
#include "utils/hash_table.h"

#include <stdint.h>


/**
 * @brief Sandbox for webasembly runtime, customised for plc.
 * 
 */
typedef struct {

    ////////////////////////////////////////////////////////////////////////////////////////////////////
    uint8_t *in;                            ///pointer to static allocation for input area
    uint32_t input_size;
    uint8_t *out;                           ///pointer to static allocation for output area
    uint32_t output_size;
    uint8_t *mark;                          ///pointer to static allocation for mark area
    uint32_t mark_size;    
    uint32_t code_mem_size;
    uint8_t *code_mem;

    HashTable table_wasm_bin;               /// Hash table for store loaded bin modules in ram. //Online project

    //WasmBinModule mod;
    //ModuleInstance module_instance;         /// instance     

    /**
     * @brief TODO This is for mannage the running user programs pou.
     * It should be inplemented later in a hash table or linked list to keep track 
     * for pou's source code loaded in Load Memory
     * 
     */
    //WasmLoadInfo wasm_loaded[2];              

}RuntimeEnv;

/**
 * @brief TODO Runtime constructor. Must be improve with parameters later.
 * 
 * @param self 
 *  
 */
void InitRuntime(RuntimeEnv *self);

void SetWorkMem(RuntimeEnv *self, uint8_t *load_mem, uint32_t load_mem_size);

#ifdef __cplusplus
    }
#endif

#endif