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

#include "webassembly/bin.h"
#include "interpreter/values.h"

#include <stdint.h>

/// Fast traverse function ////////////////////////////////////////////////////////////////////
const uint8_t * SkipExprBuf(const uint8_t *const buf, const uint32_t max_len);

const uint8_t * SkipRecSubType(const uint8_t *const buf);

const uint8_t * SkipLimitTypeBuf(const uint8_t *const buf);

const uint8_t * SkipRefTypeBuf(const uint8_t *const buf);

const uint8_t * SkipTableTypeBuf(const uint8_t *const buf);

const uint8_t * SkipLocalBuf(const uint8_t *const buf);

const uint8_t *SkipMemArgBuf(const uint8_t *const buf);

//// Type check Functions /////////////////////////////////////////////////////////////////////

uint8_t IsAbsHeadType(const uint8_t *valtype);

uint8_t IsNumericType(const uint8_t *valtype);

uint8_t IsRecType(const uint8_t *encoded_type);

uint8_t IsRefType(const uint8_t *valtype);

uint8_t IsStorageType(const uint8_t *encoded_type);

uint8_t IsValType(const uint8_t *valtype);
////////////////////////////////////////////////////////////////////////////////////////////////


/// GET binary element by id functions //////////////////////////////////////////////////////////////////////////////////////////

const uint8_t * GetSubTypeByIndex(const uint8_t * buf, uint32_t subtype_index);

/// @brief Function to get the pointer where start the function type
/// @param typesec WasmBinSection of type section
/// @param type_index index of the function type to get
/// @return index where function type start (0x60) otherwise NULL
const uint8_t * GetTypeByIndex(WasmBinSection typesec, uint32_t type_index);

const uint8_t * GetImportByIndex(WasmBinSection importsec, uint32_t import_index);

const uint8_t * GetFunctionByIndex(WasmBinSection functionsec, uint32_t function_index);

const uint8_t * GetTableByIndex(WasmBinSection tablesec, uint32_t table_index);

const uint8_t * GetMemByIndex(WasmBinSection memsec, uint32_t mem_index);

const uint8_t * GetGlobalByIndex(WasmBinSection globalsec, uint32_t global_index);

const uint8_t * GetExportByIndex(WasmBinSection exportsec, uint32_t export_index);

const uint8_t * GetElementByIndex(WasmBinSection elemsec, uint32_t elem_index);

const uint8_t * GetCodeByIndex(WasmBinSection codesec, uint32_t code_index);

const uint8_t * GetDataByIndex(WasmBinSection datasec, uint32_t data_index);

const uint8_t * GetTagByIndex(WasmBinSection tagsec, uint32_t tag_index);

///////////////////////////////////////////////////////////////////////////////////////////////////////

/// Destructure functions /////////////////////////////////////////////////////////////////////////////
WasmBinFuncType DestructureFunctionType(const uint8_t *index);

uint32_t DestructureFunctionIndex(const uint8_t *index);

WasmBinGlobal DestructureGlobal(const uint8_t *global_addr);

WasmBinMemory DestructureMemory(const uint8_t *memory_addr);

WasmBinExport DestructureExport(const uint8_t *export_addr);

WasmBinFunction DestructureCode(const uint8_t *code_addr);

WasmBinMemArg DestructureMemArg(const uint8_t *index);

StackValType DestructureStackValType(const uint8_t *index);
///////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
    }
#endif

#endif