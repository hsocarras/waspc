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
typedef enum {
    CUSTOM,         /// They are intended to be used for debugging information or third-party extensions. 5.5.3
    TYPE,           /// It decodes into a vector of function types. Spec 5.5.4.
    IMPORT,
    FUNCTION,
    TABLE,
    MEMORY,
    GLOBAL,
    EXPORT,
    START,
    ELEMENT,
    CODE,
    DATA,
    DATA_COUNT,     
} SectionId;

typedef struct {    
    
    /// Section body ( not include id and size)
    const uint8_t *section_content;
    /// Section size
    uint32_t section_size;

} WasmModuleSection;

/**
 * @brief 
 * 
 */
typedef struct {
    
    /// the package version read from the WASM file 
    uint32_t package_version;

    uint32_t type_count;
    uint32_t import_count;
    uint32_t function_count;
    uint32_t table_count;
    uint32_t memory_count;    

    uint32_t global_count;
    uint32_t export_count;
    uint32_t table_seg_count;
    /* data seg count read from data segment section */
    uint32_t data_seg_count;

    uint32_t import_function_count;
    uint32_t import_table_count;
    uint32_t import_memory_count;

    uint32_t import_global_count;    

} WasmModule;



#ifdef __cplusplus
    }
#endif

#endif