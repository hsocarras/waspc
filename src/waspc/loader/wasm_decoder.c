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


static WpError DecodeTypeSection(WasmBinSection *typesec, WasmModule *module){

    WpError result = CreateError(OK, DECODER, 22, 0);       //No error
    
    const uint8_t *index = typetsec->content;                       // pointer to byte to traverse the binary file
    const uint8_t *buf_end = buf + typesec->size;                   // pointer to end of binary module
    uint32_t type_count;                                            // auxiliary variable
    uint32_t param_count;
    DEC_UINT32_LEB128 wasm_u32;                                     // auxiliary variable to decode leb128 values
    uint8_t byte_val;
    EncodedValTypes encoded_type;

    Functype types;    

    //get type count
    wasm_u32 = DecodeLeb128UInt32(index);
    if (wasm_u32.len == 0){
        result.id = INVALID_SECTION_ID;     //TODO better error for decoding import section
        return result;                                        
    }

    index = index + wasm_u32.len;

    module->type_len = wasm_u32.value;      // type's counter
    type_count = wasm_u32.value;            // type's counter
   
    //alocating types on heap
    types = ALLOCATE(FuncType, type_count);
    module->types = types;
    //TODO Check allocation works

    //TODO maybe is good idea make something similar to string internals (Crafting interpreter's book)
    // for function signature.
    
    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    for(uint32_t i = 0; i < type_count; i++){

        //get functype (0x60)
        encoded_type = READ_BYTE();
        if (encoded_type != FUNCTYPE){
            result.id = INVALID_SECTION_ID;     //TODO better error for decoding section
            return result;                                        
        }
       
        //get parameters count        
        wasm_u32 = DecodeLeb128UInt32(index);
        if (wasm_u32.len == 0){
            result.id = INVALID_SECTION_ID;     //TODO better error for decoding import section
            return result;                                        
        }
        index = index + wasm_u32.len;
        param_count = wasm_u32.value;
        types[i].param_len = param_count;

        //allocating array for param count
        types[i].param = ALLOCATE(uint8_t, param_count);
        //TODO chec allocation

        //Loop for read parameter vector
        for(uint32_t ii = 0; ii < param_count; ii++){

            //get encoded parameter type            
            encoded_type = READ_BYTE();

            //TODO check for valid type
            types[i].param[ii] = encoded_type;
        }
        
        //getting result count
        wasm_u32 = DecodeLeb128UInt32(index);
        if (wasm_u32.len == 0){
            result.id = INVALID_SECTION_ID;     //TODO better error for decoding import section
            return result;                                        
        }
        index = index + wasm_u32.len;
        param_count = wasm_u32.value;
        types[i].result_len = param_count;

        //Loop for read result vector
        for(uint32_t ii = 0; ii < param_count; ii++){

            //get encoded parameter type            
            encoded_type = READ_BYTE();

            //TODO check for valid type
            types[i].result[ii] = encoded_type;
        }        

    }
    
    if(index != buf_end){
        //Decode fails
        result.id = INVALID_SECTION_ID;     //TODO better error for decoding import section
        return result; 
    }

    return result; 
    
    #undef READ_BYTE
    #undef NOT_END
}

/**
 * @brief 
 * 
 * @param importsec 
 * @param module 
 * @return WpError 
 */
static WpError DecodeImportSection(WasmBinSection *importsec, WasmModule *module){

    WpError result = CreateError(OK, DECODER, 133, 0);       //No error
    
    const uint8_t *index = importsec->content;                      // pointer to byte to traverse the binary file
    const uint8_t *buf_end = buf + importsec->size;                 // pointer to end of binary module
    uint32_t import_count;
    DEC_UINT32_LEB128 wasm_u32;                                     // auxiliary variable to decode leb128 values
    uint8_t byte_val;

    WasmModuleImport *imports;
    


    //get inports count
    wasm_u32 = DecodeLeb128UInt32(index);
    if (wasm_u32.len == 0){
        result.id = INVALID_SECTION_ID;     //TODO better error for decoding import section
        return result;                                        
    }

    index = index + wasm_u32.len;
    module->imports_len = wasm_u32.value;   // import's counter
    import_count = wasm_u32.value;          // import's counter
   
    //alocating imports on heap
    imports = ALLOCATE(WasmModuleImport, import_count);
    module->imports = imports;
    //TODO Check allocation works

    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    for(uint32_t i = 0; i < import_count; i++){
        //get module name's vector
        wasm_u32 = DecodeLeb128UInt32(index);
        if (wasm_u32.len == 0){
            result.id = INVALID_SECTION_ID;     //TODO better error for decoding import section
            return result;                                        
        }
        index = index + wasm_u32.len;
        imports[i].module_len = wasm_u32.value;
        imports[i].module = index;

        //moving index to name vector
        index = index + wasm_u32.value;

        //get name's vector
        wasm_u32 = DecodeLeb128UInt32(index);
        if (wasm_u32.len == 0){
            result.id = INVALID_SECTION_ID;     //TODO better error for decoding import section
            return result;                                        
        }
        index = index + wasm_u32.len;
        imports[i].name_len = wasm_u32.value;
        imports[i].name = index;

        //moving index to import desc
        index = index + wasm_u32.value;

        //Get import type
        imports[i].desc = ALLOCATE(ImportDesc, 1);
        //TODO Check allocation works

        byte_val = READ_BYTE();
        switch (byte_val){
            case 0:                
                imports[i].desc.type = FUNC;
                break;
            case 1:
                imports[i].desc.type = TABLE;
                break;
            case 2:
                imports[i].desc.type = MEM;
                break;
            case 3:
                imports[i].desc.type = GLOBAL;
                break;
            default:
                result.id = INVALID_SECTION_ID;     //TODO better error for decoding import section
                return result; 
        }

        //get desc index
        wasm_u32 = DecodeLeb128UInt32(index);
         if (wasm_u32.len == 0){
            result.id = INVALID_SECTION_ID;     //TODO better error for decoding import section
            return result;                                        
        }
        imports[i].desc.idx = wasm_u32.value;        

        //moving index to next import
        index = index + wasm_u32.len;

    }
    
    if(index != buf_end){
        //Decode fails
        result.id = INVALID_SECTION_ID;     //TODO better error for decoding import section
        return result; 
    }

    return result; 
    
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

    WpError result = CreateError(OK, DECODER, 253, 0);       //No error
    WasmBinModule bin_module = *self->mod;

    //1 - If module is not valid, then: Fail

    //2. Assert: module is valid with external types externtype𝑚 im classifying its imports.

    //3. If the number 𝑚 of imports is not equal to the number 𝑛 of provided external values, then: Fail
}