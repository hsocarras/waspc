/**
 * @file wasmbinmodule.h
 * @author Hector E. Socarras (hsocarras1987@gmail.com)
 * @brief Header file for waspc runtime base object.
 * @version 0.1
 * @date 2024-06-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef WASPC_OBJECT_BIN_MODULE_H
#define WASPC_OBJECT_BIN_MODULE_H

#ifdef __cplusplus
    extern "C" {
#endif

//wasp includes
#include "object/object.h"
#include "webassembly/binary/module.h"

#include <stdint.h>

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
typedef struct WpBinModule {
    /// head for all Waspc object to allow cast
    WpObjectType type;
    
    uint32_t bin_len;                   /// len of module
    const uint8_t * start;              /// pointer to where the module start in the Load Memory
     
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

    ///Custom section will be ignored due has no use for interpreter execution.

} WpBinModule;

// Methods *****************************************************************************************
void WpBinModuleInit(WpBinModule *self);

#ifdef __cplusplus
    }
#endif

#endif