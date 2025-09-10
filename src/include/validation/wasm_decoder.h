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

/*
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

VecData * DecodeDataSection(WpModuleState *mod);*/

/// @brief function to skip expr rule
/// expr ::= (in:instr)* 0x0B ⇒ in* end
/// @param buf pointer to beguin of expr
/// @param max_len max leng to avoid endless loop
/// @return 
const uint8_t * SkipExprBuf(const uint8_t *const buf, const uint32_t max_len);

const uint8_t * SkipLimitTypeBuf(const uint8_t *const buf);

/// @brief Function to get the pointer where start the function type
/// @param typesec WasmBinSection of type section
/// @param type_index index of the function type to get
/// @return index where function type start (0x60) otherwise NULL
const uint8_t * GetFuncTypeByIndex(WasmBinSection typesec, uint32_t type_index);

const uint8_t * GetImportByIndex(WasmBinSection importsec, uint32_t import_index);

const uint8_t * GetFunctionByIndex(WasmBinSection functionsec, uint32_t function_index);

const uint8_t * GetTableByIndex(WasmBinSection tablesec, uint32_t table_index);

const uint8_t * GetMemByIndex(WasmBinSection memsec, uint32_t mem_index);

const uint8_t * GetGlobalByIndex(WasmBinSection globalsec, uint32_t global_index);

const uint8_t * GetExportByIndex(WasmBinSection exportsec, uint32_t export_index);

const uint8_t * GetElementByIndex(WasmBinSection elemsec, uint32_t elem_index);

const uint8_t * GetCodeByIndex(WasmBinSection codesec, uint32_t code_index);

const uint8_t * GetDataByIndex(WasmBinSection datasec, uint32_t data_index);

#ifdef __cplusplus
    }
#endif

#endif