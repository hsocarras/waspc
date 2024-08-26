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

// Encoded Types///////////////////////////////////////////////////////////////////////////////////////

typedef enum EncodedValTypes {

    FUNCTYPE = 0x60,
    EXTERN_REF_TYPE = 0x6F,
    FUNC_REF_TYPE = 0x70,
    V128 = 0x7B,
    F64 = 0x7C,
    F32 = 0x7D,
    I64 = 0x7E,
    I32 = 0x7F,

} EncodedValTypes;

//#####################################################################################################

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

typedef struct WasmBinSection {   

    /// Section size
    uint32_t size;
    /// Section body ( not include id and size)
    const uint8_t *content;
    

} WasmBinSection;

/**
 * @brief The encoding of a module starts with a preamble containing a 4-byte magic number (the string ‘∖0asm’) and a
 *         version field. The current version of the WebAssembly binary format is 1.
 *              The preamble is followed by a sequence of sections. Custom sections may be inserted at any place in this sequence,
 *          while other sections must occur at most once and in the prescribed order. All sections can be empty
 *
 *          
 *              magic ::= 0x00 0x61 0x73 0x6D
                version ::= 0x01 0x00 0x00 0x00
                module ::= magic
                version
                customsec*
                functype*: typesec
                customsec*
                import*: importsec
                customsec*
                typeidx𝑛: funcsec
                customsec*
                table*: tablesec
                customsec*
                mem*: memsec
                customsec*
                global*: globalsec
                customsec*
                export*: exportsec
                customsec*
                start?: startsec
                customsec*
                elem*: elemsec
                customsec*
                𝑚?: datacountsec
                customsec*
                code𝑛: codesec
                customsec*
                data𝑚: datasec
                customsec* ⇒ { ...)
 */
typedef struct WasmBinModule {
    /// This struct will be loaded as base executable in work memory
    /// only wasm's essential parts will be loaded.
    /// custom section are not, and also magic number.

    uint32_t bin_len;                   /// len of module
    uint8_t *start;                     /// pointer to where the module start in the Load Memory
     
    uint32_t version;                   /// the version read from the WASM file    
    WasmBinSection typesec;             /// encoded type section
    WasmBinSection importsec;           /// encoded import section
    WasmBinSection functionsec;         /// encoded func section
    WasmBinSection tablesec;            /// encoded table section
    WasmBinSection memsec;              /// encoded memory section
    WasmBinSection globalsec;           /// encoded global section
    WasmBinSection exportsec;           /// encoded export section
    WasmBinSection startsec;            /// encoded start section
    WasmBinSection elemsec;             /// encoded element section
    WasmBinSection datacountsec;        /// encoded data count section section
    WasmBinSection codesec;             /// encoded code section
    WasmBinSection datasec;             /// encoded data section  

} WasmBinModule;



#ifdef __cplusplus
    }
#endif

#endif