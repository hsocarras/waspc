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

//WASPC includes
#include "webassembly/binary/module.h"

#include <stdint.h>



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