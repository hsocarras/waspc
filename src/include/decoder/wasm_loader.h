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
#include "object/result.h"
#include "object/bin_module.h"
#include "object/decoded_module.h"

#include <stdint.h>



/**
 * Function to create a WasmBinModule from a byte array
 * 
 */
WpResult LoadWasmBuffer(const uint8_t * const buf, const uint32_t size, WpBinModule *bin_mod);


//WpError InstanciateModule(RuntimeEnv *self, WasmBinModule *mod, uint8_t *imports, uint32_t import_counts, const uint32_t id);



#ifdef __cplusplus
    }
#endif

#endif