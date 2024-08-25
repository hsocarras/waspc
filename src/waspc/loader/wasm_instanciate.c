/**
 * @file wasm_instanciate.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-21
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "runtime/runtime.h"
#include "loader/wasm_loader.h"
#include "diagnostic/error.h"
#include "webassembly/binary/module.h"
#include "webassembly/structure/module.h"
#include "utils/leb128.h"

#include <stdint.h>


static WpError DecodeImportSection(WasmBinSection *importsec, WasmModule *module){

    WpError result = CreateError(OK);       //No error
    const uint8_t *index = importsec->content;                      // pointer to byte to traverse the binary file
    const uint8_t *buf_end = buf + importsec->size;                 // pointer to end of binary module
    DEC_UINT32_LEB128 wasm_u32;                                     // auxiliary variable to decode leb128 values


    //get inports count
    wasm_u32 = DecodeLeb128UInt32(index);
    if (wasm_u32.len == 0){
        result.id = INVALID_SECTION_ID;     //TODO better error for decoding import section
        return result;                                        
    }

    module->imports_len = wasm_u32.value;

    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    
    
    #undef READ_BYTE
    #undef NOT_END
}

/**
 * @brief 
 * 
 * @param self 
 * @param mod 
 * @param imports 
 * @param import_counts 
 * @param id 
 * @return WpError 
 */
WpError InstanciateModule(RuntimeEnv *self, WasmBinModule *mod, uint8_t *imports, uint32_t import_counts, const uint32_t id){

    WpError result = CreateError(OK);       //No error
    WasmBinModule bin_module = *self->mod;

    //1 - If module is not valid, then: Fail

    //2. Assert: module is valid with external types externtype𝑚 im classifying its imports.

    // If the number 𝑚 of imports is not equal to the number 𝑛 of provided external values, then: Fail
}