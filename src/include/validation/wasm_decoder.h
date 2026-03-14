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

#include <stdint.h>

/**
 * @brief Enum with section id number inside binary format.
 * 
 */


//// Type check Functions /////////////////////////////////////////////////////////////////////

uint8_t IsAbsHeadType(uint8_t valtype);

uint8_t IsRecType(uint8_t byte);

uint8_t isRefType(uint8_t encoded_type);

uint8_t isStorageType(uint8_t encoded_type);

uint8_t IsValType(uint8_t val_type);
////////////////////////////////////////////////////////////////////////////////////////////////

/// SKIP: Function to skip a buffer corresponding to an expression//////////////////////////////
// TODO check for make static skip functions
const uint8_t * SkipExprBuf(const uint8_t *const buf, const uint32_t max_len);

const uint8_t * SkipRecSubType(const uint8_t *const buf);

const uint8_t * SkipLimitTypeBuf(const uint8_t *const buf);

const uint8_t * SkipRefTypeBuf(const uint8_t *const buf);

const uint8_t * SkipTableTypeBuf(const uint8_t *const buf);
///////////////////////////////////////////////////////////////////////////////////////////////

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

WasmBinFunction DestructureCode(const uint8_t *code_addr);
///////////////////////////////////////////////////////////////////////////////////////////////////////

#ifdef __cplusplus
    }
#endif

#endif