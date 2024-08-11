/**
 * @file wasm_loader.h
 * @author Hector E. Socarras (hsocarras1987@gmail.com)
 * @brief Header file for wasm load module
 * @version 0.1
 * @date 2024-06-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef WASPC_WASM_LOADER_H
#define WASPC_WASM_LOADER_H

#ifdef __cplusplus
    extern "C" {
#endif

//wasp includes
#include "vm.h"
#include "diagnostic/error.h"

#include <stdint.h>



/**
 * @brief 
 * 
 */
WpError LoadWasm(Vm *, const uint8_t *buf, uint32_t size);

#ifdef __cplusplus
    }
#endif

#endif