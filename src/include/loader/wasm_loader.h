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
#include "runtime/runtime.h"
#include "diagnostic/error.h"

#include <stdint.h>



/**
 * @brief 
 * 
 */
WpError LoadWasmBuffer(RuntimeEnv *self, const uint8_t *buf, uint32_t size, const uint32_t id);


WpError InstanciateModule(RuntimeEnv *self, WasmBinModule *mod, uint8_t *imports, uint32_t import_counts, const uint32_t id);

WpError DecodeWasmBinModule(RuntimeEnv *self, WasmBinModule *bin_mod, WasmModule *mod);

#ifdef __cplusplus
    }
#endif

#endif