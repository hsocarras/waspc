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

#ifndef WASPC_WASM_DECODER_H
#define WASPC_WASM_DECODER_H

#ifdef __cplusplus
    extern "C" {
#endif

//wasp includes
#include "object/result.h"
#include "object/bin_module.h"
#include "object/decoded_module.h"

#include <stdint.h>

WpResult DecodeTypeSection(const WasmBinSection *const sec, WpDecodedModule *mod);

WpResult DecodeImportSection(const WasmBinSection *const sec, WpDecodedModule *mod);

WpResult DecodeFunctionSection(const WasmBinSection *const sec, WpDecodedModule *mod);

WpResult DecodeTableSection(const WasmBinSection *const sec, WpDecodedModule *mod);

WpResult DecodeMemSection( const WasmBinSection *const sec, WpDecodedModule *mod);

WpResult DecodeGlobalSection(const WasmBinSection *const sec, WpDecodedModule *mod);

WpResult DecodeExportSection(const WasmBinSection *const sec, WpDecodedModule *mod);

WpResult DecodeStartSection(const WasmBinSection *const sec, WpDecodedModule *mod);

WpResult DecodeElementSection(const WasmBinSection *const sec, WpDecodedModule *mod);

WpResult DecodeDataCountSection(const WasmBinSection *const sec, WpDecodedModule *mod);

WpResult DecodeCodeSection(const WasmBinSection *const sec, WpDecodedModule *mod);

WpResult DecodeDataSection(const WasmBinSection * const sec, WpDecodedModule *mod);

WpResult DecodeWpBinModule(const WpBinModule * const bin_mod, WpDecodedModule *mod);






#ifdef __cplusplus
    }
#endif

#endif