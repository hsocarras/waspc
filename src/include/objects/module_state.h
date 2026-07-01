/**
 * @file instance.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-12-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */
#ifndef WASPC_OBJECTS_MODULE_STATE_H
#define WASPC_OBJECTS_MODULE_STATE_H

#ifdef __cplusplus
    extern "C" {
#endif

//wasp includes
#include "objects/object.h"
#include "objects/module_instance.h"
#include "webassembly/bin.h"



#include <stdint.h>


typedef enum WpModuleStatus{
    WP_MODULE_STATUS_INIT,
    WP_MODULE_STATUS_ERROR,
    WP_MODULE_STATUS_INVALID,
    WP_MODULE_STATUS_VALIDATED,
    WP_MODULE_STATUS_INSTANTIATED,    
} WpModuleStatus;



/**
 * @version 3.0
 * @brief Result object for return values for most waspc internal functions.
 * 
 */
typedef struct WpModuleState{
    /// head for all Waspc object to allow cast
    WpObjectType wp_type;     

    /// @brief module's name.
    //Name name;

    /// @brief module status
    WpModuleStatus status;

    /**
     * @brief The encoding of a module starts with a preamble containing a 4-byte magic number (the string ‘∖0asm’) and a
     *         version field. The current version of the WebAssembly binary format is 1.
     *              The preamble is followed by a sequence of sections. Custom sections may be inserted at any place in this sequence,
     *          while other sections must occur at most once and in the prescribed order. All sections can be empty
     * 
     * */
    const uint8_t *buf;                 /// pointer to binary buffer wasm    
    uint32_t bufsize;                   /// size of the buffer
    uint32_t version;                   /// the version read from the WASM file 
    
    ///Binary sections start and size, these are for internal use only, not for public API. They are used to get the section content without decode them into WasmSection.
    WasmBinSection typesec;             /// encoded type section
    WasmBinSection importsec;           /// encoded import section
    WasmBinSection functionsec;         /// encoded func section
    WasmBinSection tablesec;            /// encoded table section
    WasmBinSection memsec;              /// encoded memory section
    WasmBinSection tagsec;              /// encoded tag section
    WasmBinSection globalsec;           /// encoded global section
    WasmBinSection exportsec;           /// encoded export section
    WasmBinSection startsec;            /// encoded start section
    WasmBinSection elemsec;             /// encoded element section
    WasmBinSection datacountsec;        /// encoded data count section section
    WasmBinSection codesec;             /// encoded code section
    WasmBinSection datasec;             /// encoded data section

    //Pointer to corresponding instace on store
    WpModuleInstance *instance;

} WpModuleState;

// Methods **************************************************************************************************

void WpModuleStateInit(WpModuleState *self);



#ifdef __cplusplus
    }
#endif

#endif