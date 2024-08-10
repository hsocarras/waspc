/**
 * @file loader.h
 * @author Hector E. Socarras (hsocarras1987@gmail.com)
 * @brief Header file for wasm load module
 * @version 0.1
 * @date 2024-06-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef WASPC_RUNTIME_LOADER_H
#define WASPC_RUNTIME_LOADER_H

#ifdef __cplusplus
    extern "C" {
#endif

#include "loader/wasm_loader.h"

#include <stdint.h>

/**
 * @brief function to load a binary wasm file into waspc runtime
 * 
 * @param buf binary webassembly module in raw byte array.
 * @param size binary webassembly module byte array's length
 * @param error_buf TODO
 * @param error_buf_size TODO
 * @return WASM_Module 
 */
WASM_Module wasm_runtime_load(uint8_t *buf, uint32_t size, uint8_t *error_buf, uint32_t error_buf_size);


#ifdef __cplusplus
    }
#endif

#endif