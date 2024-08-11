/**
 * @file vm.h
 * @author Hector E. Socarras (hsocarras1987@gmail.com)
 * @brief Header file for webassembly virtual machine
 * @version 0.1
 * @date 2024-06-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef WASPC_VM_H
#define WASPC_VM_H

#ifdef __cplusplus
    extern "C" {
#endif



#include <stdint.h>

/**
 * @brief 
 * 
 */
typedef struct {
    
    /* the package version read from the WASM file */
    uint32_t package_version;

    uint32_t type_count;
    uint32_t import_count;
    uint32_t function_count;
    uint32_t table_count;
    uint32_t memory_count;    

    uint32_t global_count;
    uint32_t export_count;
    uint32_t table_seg_count;
    /* data seg count read from data segment section */
    uint32_t data_seg_count;

    uint32_t import_function_count;
    uint32_t import_table_count;
    uint32_t import_memory_count;

    uint32_t import_global_count;    

} WasmModule;

typedef struct {

    ///dummy for now
    char dummy;

}VmState;

typedef struct {
    
    /// Interpreter State
    VmState state;

    //Temporary store for webassembly module
    WasmModule main_module;

} Vm;

#ifdef __cplusplus
    }
#endif

#endif