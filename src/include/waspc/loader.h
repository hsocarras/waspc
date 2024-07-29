/**
 * @file loader.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-07-21
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef AOS_VM_LOADER_H
#define AOS_VM_LOADER_H

#ifdef __cplusplus
    extern "C" {
#endif

#include "vm/vm.h"

#include <stdint.h>

/**
 * @brief Enum for loading process status report
 * 
 */
typedef enum {
    LOAD_OK,                    /// Load ok
    ERR_MAGIC,                  /// Error with magic number
    ERR_VERSION,                /// Error with binary format version
    ERR_SECTION_ID,
} LoaderResult;

/**
 * @brief Struct to implement a binary module representation direct from binary file
 * 
 */
typedef struct {
    LoaderResult load_status;                       /// True if magic and version number is ok and section was identified ok
    uint8_t *custom_section;                        /// pointer to where custom section start (byte with section id = 0)
    uint32_t custom_section_len                     /// custom section's len decode fron leb128
    uint8_t *type_section;                          /// pointer to where type section start (byte with section id = 1)
    uint32_t type_section_len                       /// type section's len decode fron leb128
    uint8_t *import_section;                        /// pointer to where import section start (byte with section id = 2)
    uint32_t import_section_len                     /// import section's len decode fron leb128
    uint8_t *function_section;                      /// pointer to where function section start (byte with section id = 3)
    uint32_t function_section_len                   /// function section's len decode fron leb128
    uint8_t *table_section;                         /// pointer to where table section start (byte with section id = 4)
    uint32_t table_section_len                      /// table section's len decode fron leb128
    uint8_t *memory_section;                        /// pointer to where memory section start (byte with section id = 5)
    uint32_t memory_section_len                     /// memory section's len decode fron leb128
    uint8_t *global_section;                        /// pointer to where global section start (byte with section id = 6)
    uint32_t global_section_len                     /// global section's len decode fron leb128
    uint8_t *export_section;                        /// pointer to where export section start (byte with section id = 7)
    uint32_t export_section_len                     /// export section's len decode fron leb128
    uint8_t *start_section;                         /// pointer to where start section start (byte with section id = 8)
    uint32_t start_section_len                      /// start section's len decode fron leb128
    uint8_t *element_section;                       /// pointer to where element section start (byte with section id = 9)
    uint32_t element_section_len                    /// element section's len decode fron leb128
    uint8_t *code_section;                          /// pointer to where code section start (byte with section id = 10)
    uint32_t code_section_len                       /// code section's len decode fron leb128
    uint8_t *data_section;                          /// pointer to where data section start (byte with section id = 11)
    uint32_t data_section_len                       /// data section's len decode fron leb128
    uint8_t *data_count_section;                    /// pointer to where data count section start (byte with section id = 12)
    uint32_t data_count_section_len                 /// data count section's len decode fron leb128

}BinaryModule;

#define NULL_BINARY_MODULE {.load_status=LOAD_OK, .custom_section=null, .code_section_len=0, .type_section=null, \
                            .type_section_len = 0, .import_section=null, .import_section_len=0\
                            .function_section=null, .function_section_len=0, .table_section=null, .table_section_len=0\
                            .memory_section=null, .memory_section_len=0, .global_section=null, .global_section_len=0\
                            .export_section=null, .export_section_len=0, .start_section=null, .start_section_len=0\
                            .element_section=null, .element_section_len=0, .code_section=null, .code_section_len=0\
                            .data_section=null, .data_section_len=0, .data_count_section=null, .data_count_section_len=0}


///// LOADING FUNCTIONS ////////////////////////////////////////////////////////////////////////////////////////



#ifdef __cplusplus
    }
#endif

#endif
