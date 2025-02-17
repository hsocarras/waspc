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
#include "objects/module.h"

#include <stdint.h>

VecFuncType *DecodeTypeSection(WpModuleState *mod);

VecImport * DecodeImportSection(WpModuleState *mod);

VecFunc * DecodeFunctionSection(WpModuleState *mod);

VecTable * DecodeTableSection(WpModuleState *mod);

VecMem * DecodeMemSection(WpModuleState *mod);

VecGlobal * DecodeGlobalSection(WpModuleState *mod);

VecExport * DecodeExportSection(WpModuleState *mod);

uint32_t * DecodeStartSection(WpModuleState *mod);

VecElem * DecodeElementSection(WpModuleState *mod);

uint32_t * DecodeDataCountSection(WpModuleState *mod);

VecFunc * DecodeCodeSection(WpModuleState *mod);

VecData * DecodeDataSection(WpModuleState *mod);


#ifdef __cplusplus
    }
#endif

#endif