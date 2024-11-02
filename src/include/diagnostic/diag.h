/**
 * @file error.h
 * @author Hector E. Socarras (hsocarras1987@gmail.com)
 * @brief Header file for diagnostic definitions
 * @version 0.1
 * @date 2024-06-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef WASPC_DIAGNOSTIC_DIAG_H
#define WASPC_DIAGNOSTIC_DIAG_H

#ifdef __cplusplus
    extern "C" {
#endif


#include <stdint.h>

/**
 * @brief Enum to identify all posible error
 * 
 */
typedef enum WpDiagId{

    WP_DIAG_ID_ALLOCATION_FAILURE,                          //ID 0
    WP_DIAG_ID_DECODE_LEB128_FAIL,
    WP_DIAG_ID_INVALID_MODULE_SIZE,
    WP_DIAG_ID_INVALID_MAGIC,
    WP_DIAG_ID_INVALID_VERSION,
    WP_DIAG_ID_UNEXPECTED_SECTION_ID, 
    WP_DIAG_ID_INVALID_ENCODED_VALUE,
    WP_DIAG_ID_ERROR_DECODE_TYPE_SECTION,               
    WP_DIAG_ID_INVALID_IMPORT_DESC_TYPE,                    //ID 8
    WP_DIAG_ID_ERROR_DECODE_IMPORT_SECTION,
    WP_DIAG_ID_ERROR_DECODE_FUNCTION_SECTION,
    WP_DIAG_ID_INVALID_REFERENCE_TYPE,
    WP_DIAG_ID_INVALID_LIMITS_TYPE,
    WP_DIAG_ID_ERROR_DECODE_TABLE_SECTION,
    WP_DIAG_ID_ERROR_DECODE_MEM_SECTION,
    WP_DIAG_ID_INVALID_GLOBAL_TYPE,
    WP_DIAG_ID_DECODE_EXPR_FAIL,                            //ID 16
    WP_DIAG_ID_ERROR_DECODE_GLOBAL_SECTION,
    WP_DIAG_ID_INVALID_EXPORT_DESCRIPTION_TYPE,
    WP_DIAG_ID_ERROR_DECODE_EXPORT_SECTION,
    WP_DIAG_ID_ERROR_DECODE_START_SECTION,
    WP_DIAG_ID_INVALID_ELEMENT_TYPE,
    WP_DIAG_ID_ERROR_DECODE_ELEM_SECTION,
    WP_DIAG_ID_ASSERT_DECODE_CODE_SECTION,
    WP_DIAG_ID_DECODE_CODE_FAIL,                            //ID 24
    WP_DIAG_ID_ERROR_DECODE_CODE_SECTION,
    WP_DIAG_ID_ERROR_DECODE_DATA_COUNT_SECTION,
    WP_DIAG_ID_ERROR_DECODE_DATA_SECTION,
    WP_DIAG_ID_INVALID_DATA_MODE,
    WP_DIAG_ID_ASSERT_DECODE_DATA_SECTION,
    WP_DIAG_ID_TODO,
} WpDiagId;

/**
 * @brief enum with all module that can produce an error
 * Every .c file is considered a module.
 */
typedef enum WpDiagModuleList {
    W_DIAG_MOD_LIST_EMBEDDER,
    W_DIAG_MOD_LIST_LOADER,
    W_DIAG_MOD_LIST_DECODER,
    W_DIAG_MOD_LIST_UTILS,
} WpDiagModuleList;



#ifdef __cplusplus
    }
#endif

#endif