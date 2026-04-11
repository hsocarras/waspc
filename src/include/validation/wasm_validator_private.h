/**
 * @file wasm_validator_private.h
 * @author your name (you@domain.com)
 * @brief  Header file with validation module's private function
 * @version 0.1
 * @date 2024-09-22
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef WASPC_WASM_VALIDATOR_PRIVATE_H
#define WASPC_WASM_VALIDATOR_PRIVATE_H

#ifdef __cplusplus
    extern "C" {
#endif

//wasp includes
#include "validation/wasm_validator.h"


/// Methods (Functions with self) ///////////////////////////////////////////////////////////////////////////////////////

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
/// Auxiliary functions for validation of sections.
/// They are called by the main validation function when validating each section.
/// They are implemented in the wasm_validator_buffer.c file and are used to validate the binary syntax of each section's content.

uint32_t ValidateMagicBuf(const uint8_t *buf);

uint32_t ValidateVersionBuf(const uint8_t *buf, uint32_t *version_number);

uint32_t ValidateCompTypeBuf(const uint8_t *buf);

uint32_t ValidateSubtypeBuf(const uint8_t *buf, uint32_t type_count);

uint32_t ValidateRefTypeBuf(const uint8_t *buf);

uint32_t ValidateMemTypeBuf(const uint8_t *buf);

uint32_t ValidateTagTypeBuf(const uint8_t *buf);

uint32_t ValidateExternalTypeBuf(const uint8_t *buf);

uint32_t ValidateTableBuf(const uint8_t *buf);

uint32_t ValidateTypeBuf(const uint8_t *buf, uint32_t type_count);

uint32_t ValidateImportBuf(const uint8_t *buf, uint32_t functiontype_count);

uint32_t ValidateTableTypeBuf(const uint8_t *buf);

uint32_t ValidateLimitsTypeBuf(const uint8_t *buf, uint32_t k);

uint32_t ValidateGlobalTypeBuf(const uint8_t *buf);

uint32_t ValidateExportBuf(const uint8_t *buf);

uint32_t ValidateElementBuf(const uint8_t *buf, uint32_t function_count, uint32_t table_count);

uint32_t ValidateCodeBuf(const uint8_t *buf);

uint32_t ValidateDataBuf(const uint8_t *buf, uint32_t memory_count);

uint32_t ValidateConstantExprBuf(const uint8_t *buf, uint32_t max_len);

uint32_t ValidateGlobalBuf(const uint8_t *buf, const uint8_t *global_section_end);

uint32_t ValidateTableBuf(const uint8_t *buf);

////////////////////////////////////////////////////////////////////////////////////////////////////////////////////




#ifdef __cplusplus
    }
#endif

#endif