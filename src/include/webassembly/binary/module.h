/**
 * @file module.h
 * @author Hector E. Socarras (hsocarras1987@gmail.com)
 * @brief Header file for webassembly virtual machine
 * @version 0.1
 * @date 2024-06-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef WASPC_WEBASSEMBLY_BIN_MODULE_H
#define WASPC_WEBASSEMBLY_BIN_MODULE_H

#ifdef __cplusplus
    extern "C" {
#endif

#include <stdint.h>



/**
 * @brief Enum with section id number inside binary format.
 * 
 */
typedef enum SectionId{
    WP_WSA_BIN_MOD_SEC_ID_CUSTOM,         /// They are intended to be used for debugging information or third-party extensions. 5.5.3
    WP_WSA_BIN_MOD_SEC_ID_TYPE,           /// It decodes into a vector of function types. Spec 5.5.4.
    WP_WSA_BIN_MOD_SEC_ID_IMPORT,
    WP_WSA_BIN_MOD_SEC_ID_FUNCTION,
    WP_WSA_BIN_MOD_SEC_ID_TABLE,
    WP_WSA_BIN_MOD_SEC_ID_MEMORY,
    WP_WSA_BIN_MOD_SEC_ID_GLOBAL,
    WP_WSA_BIN_MOD_SEC_ID_EXPORT,
    WP_WSA_BIN_MOD_SEC_ID_START,
    WP_WSA_BIN_MOD_SEC_ID_ELEMENT,
    WP_WSA_BIN_MOD_SEC_ID_CODE,
    WP_WSA_BIN_MOD_SEC_ID_DATA,
    WP_WSA_BIN_MOD_SEC_ID_DATA_COUNT,     
} SectionId;

typedef struct WasmBinSection {   
    /// Section size
    uint32_t size;
    /// Section body ( not include id and size)
    const uint8_t *content;
} WasmBinSection;





#ifdef __cplusplus
    }
#endif

#endif