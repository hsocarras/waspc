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
#include "memory/memory.h"
#include "diagnostic/error.h"
#include "webassembly/binary/module.h"
#include "webassembly/binary/opcode.h"
#include "webassembly/structure/types.h"
#include "webassembly/structure/module.h"
#include "utils/leb128.h"

#include <stdint.h>

static const uint8_t * DecodedLimitsType(const uint8_t *buf, Limits *lim){

    const uint8_t *index = buf;    
    DEC_UINT32_LEB128 wasm_u32;                                                     // auxiliary variable to decode leb128 values
    uint8_t byte_val;

    #define READ_BYTE() (*index++)

    //get index flag for maximun precense
    byte_val = READ_BYTE();
    if(byte_val == 0){
        //maximun not present
        wasm_u32 = DecodeLeb128UInt32(index);           //get minimun value
        if (wasm_u32.len == 0){
            return NULL;                                        
        }
        index = index + wasm_u32.len;
        lim->min = wasm_u32.value;
        lim->max = 0;
        return index;
    }
    else if (byte_val == 1){
        //maximun present
        wasm_u32 = DecodeLeb128UInt32(index);           //get minimun value
        if (wasm_u32.len == 0){
            return NULL;                                        
        }
        index = index + wasm_u32.len;
        lim->min = wasm_u32.value;
        
        wasm_u32 = DecodeLeb128UInt32(index);           //get maximun value
        if (wasm_u32.len == 0){
            return NULL;                                        
        }
        index = index + wasm_u32.len;
        lim->max = wasm_u32.value;
        return index;
    }
    else{
        //error encoded limit
        return NULL;                   //means error otherwise the limits len to shift the index in caller.
    }

    #undef READ_BYTE

}

static WpError DecodeTypeSection(WasmBinSection *sec, WasmModule *module){

    WpError result = CreateError(WP_DIAG_ID_OK, W_DIAG_MOD_LIST_DECODER, 22, 0);       //No error
    
    const uint8_t *index = sec->content;                       // pointer to byte to traverse the binary file
    const uint8_t *buf_end = sec->content + sec->size;                   // pointer to end of binary module
    uint32_t type_count;                                            // auxiliary variable
    uint32_t param_count;
    DEC_UINT32_LEB128 wasm_u32;                                     // auxiliary variable to decode leb128 values
    uint8_t byte_val;
    EncodedValTypes encoded_type;

    FuncType *types;    

    //get type count
    wasm_u32 = DecodeLeb128UInt32(index);
    if (wasm_u32.len == 0){
        result.id = WP_DIAG_ID_INVALID_SECTION_ID;     //TODO better error for decoding import section
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
        if (encoded_type != WP_WAS_BIN_ENC_FUNCTYPE){
            result.id = WP_DIAG_ID_INVALID_SECTION_ID;     //TODO better error for decoding section            
            return result;                                        
        }
        
        //get parameters count        
        wasm_u32 = DecodeLeb128UInt32(index);
        if (wasm_u32.len == 0){
            result.id = WP_DIAG_ID_INVALID_SECTION_ID;     //TODO better error for decoding import section
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
            result.id = WP_DIAG_ID_INVALID_SECTION_ID;     //TODO better error for decoding import section           
            return result;                                        
        }
        index = index + wasm_u32.len;
        param_count = wasm_u32.value;
        types[i].result_len = param_count;
        
        //allocating array for result count
        types[i].result = ALLOCATE(uint8_t, param_count);
        //TODO chec allocation

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
        result.id = WP_DIAG_ID_INVALID_SECTION_ID;     //TODO better error for decoding import section
        result.detail.place = 114;
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
static WpError DecodeImportSection(WasmBinSection *sec, WasmModule *module){

    WpError result = CreateError(WP_DIAG_ID_OK, W_DIAG_MOD_LIST_DECODER, 133, 0);       //No error
    
    const uint8_t *index = sec->content;                      // pointer to byte to traverse the binary file
    const uint8_t *buf_end = sec->content + sec->size;                 // pointer to end of binary module
    uint32_t import_count;
    DEC_UINT32_LEB128 wasm_u32;                                     // auxiliary variable to decode leb128 values
    uint8_t byte_val;

    WasmModuleImport *imports;
    


    //get inports count
    wasm_u32 = DecodeLeb128UInt32(index);
    if (wasm_u32.len == 0){
        result.id = WP_DIAG_ID_INVALID_SECTION_ID;     //TODO better error for decoding import section
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
            result.id = WP_DIAG_ID_INVALID_SECTION_ID;     //TODO better error for decoding import section
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
            result.id = WP_DIAG_ID_INVALID_SECTION_ID;     //TODO better error for decoding import section
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
                imports[i].desc->type = WP_WAS_STRUC_MOD_IMPORT_DESC_TYPE_FUNC;
                break;
            case 1:
                imports[i].desc->type = WP_WAS_STRUC_MOD_IMPORT_DESC_TYPE_TABLE;
                break;
            case 2:
                imports[i].desc->type = WP_WAS_STRUC_MOD_IMPORT_DESC_TYPE_MEM;
                break;
            case 3:
                imports[i].desc->type = WP_WAS_STRUC_MOD_IMPORT_DESC_TYPE_GLOBAL;
                break;
            default:
                result.id = WP_DIAG_ID_INVALID_SECTION_ID;     //TODO better error for decoding import section
                return result; 
        }

        //get desc index
        wasm_u32 = DecodeLeb128UInt32(index);
         if (wasm_u32.len == 0){
            result.id = WP_DIAG_ID_INVALID_SECTION_ID;     //TODO better error for decoding import section
            return result;                                        
        }
        imports[i].desc->idx = wasm_u32.value;        

        //moving index to next import
        index = index + wasm_u32.len;

    }
    
    if(index != buf_end){
        //Decode fails
        result.id = WP_DIAG_ID_INVALID_SECTION_ID;     //TODO better error for decoding import section
        result.detail.place = 230;
        return result; 
    }

    return result; 
    
    #undef READ_BYTE
    #undef NOT_END
}

static WpError DecodeFunctionSection(WasmBinSection *sec, WasmModule *module){

    WpError result = CreateError(WP_DIAG_ID_OK, W_DIAG_MOD_LIST_DECODER, 22, 0);       //No error
    
    const uint8_t *index = sec->content;                                        // pointer to byte to traverse the binary file
    const uint8_t *buf_end = sec->content + sec->size;                      // pointer to end of binary module
    uint32_t func_count;                                                            // auxiliary variable
    
    DEC_UINT32_LEB128 wasm_u32;                                                     // auxiliary variable to decode leb128 values
    uint8_t byte_val;
    EncodedValTypes encoded_type;

    Func *funcs;    

    //get functions count
    wasm_u32 = DecodeLeb128UInt32(index);
    if (wasm_u32.len == 0){
        result.id = WP_DIAG_ID_INVALID_SECTION_ID;     //TODO better error for decoding import section
        return result;                                        
    }

    index = index + wasm_u32.len;

    module->func_len = wasm_u32.value;      // type's counter
    func_count = wasm_u32.value;            // type's counter
   
    //alocating types on heap
    funcs = ALLOCATE(Func, func_count);
    module->funcs = funcs;
    //TODO Check allocation works    
        
    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    for(uint32_t i = 0; i < func_count; i++){

        
        
        //get parameters count        
        wasm_u32 = DecodeLeb128UInt32(index);
        if (wasm_u32.len == 0){
            result.id = WP_DIAG_ID_INVALID_SECTION_ID;     //TODO better error for decoding import section
            return result;                                        
        }
        index = index + wasm_u32.len;        
        funcs[i].idx = wasm_u32.value;

    }
    
    if(index != buf_end){
        //Decode fails
        result.id = WP_DIAG_ID_INVALID_SECTION_ID;     //TODO better error for decoding import section
        result.detail.place = 300;
        return result; 
    }

    return result; 
    
    #undef READ_BYTE
    #undef NOT_END
}

static WpError DecodeTableSection(WasmBinSection *sec, WasmModule *module){

    WpError result = CreateError(WP_DIAG_ID_OK, W_DIAG_MOD_LIST_DECODER, 22, 0);       //No error
    
    const uint8_t *index = sec->content;                                        // pointer to byte to traverse the binary file
    const uint8_t *buf_end = sec->content + sec->size;                      // pointer to end of binary module
    uint32_t table_count;                                                            // auxiliary variable
    
    DEC_UINT32_LEB128 wasm_u32;                                                     // auxiliary variable to decode leb128 values
    uint8_t byte_val;
    
    Table *tables;    
    Limits lim;

    //get table count
    wasm_u32 = DecodeLeb128UInt32(index);
    if (wasm_u32.len == 0){
        result.id = WP_DIAG_ID_INVALID_SECTION_ID;     //TODO better error for decoding import section
        return result;                                        
    }

    index = index + wasm_u32.len;

    module->table_len = wasm_u32.value;      // type's counter
    table_count = wasm_u32.value;            // type's counter
   
    //alocating types on heap
    tables = ALLOCATE(Table, table_count);
    module->tables = tables;
    //TODO Check allocation works    
        
    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    for(uint32_t i = 0; i < table_count; i++){        
        
        //get reference type       0x6F->external, 0x70->function ref
        byte_val = READ_BYTE();

        if(byte_val == 0x6F){
            //external reference
            //TODO
        }
        else if(byte_val == 0x70){
            //function reference
            //TODO
        }
        else{
            //Error
            //Decode fails
            result.id = WP_DIAG_ID_INVALID_SECTION_ID;     //TODO better error for decoding import section
            result.detail.place = 359;
            return result; 
        }
        //init table usage
        tables[i].usage = 0;
        index = DecodedLimitsType(index, &tables[i].lim);

        if(index == NULL){
             //Error
            //Decode fails
            result.id = WP_DIAG_ID_INVALID_SECTION_ID;     //TODO better error for decoding import section
            result.detail.place = 421;
            return result; 
        }
        

    }
    
    if(index != buf_end){
        //Decode fails
        result.id = WP_DIAG_ID_INVALID_SECTION_ID;     //TODO better error for decoding import section
        result.detail.place = 300;
        return result; 
    }

    return result; 
    
    #undef READ_BYTE
    #undef NOT_END
}

static WpError DecodeMemSection(WasmBinSection *sec, WasmModule *module){

    WpError result = CreateError(WP_DIAG_ID_OK, W_DIAG_MOD_LIST_DECODER, 22, 0);       //No error
    
    const uint8_t *index = sec->content;                                        // pointer to byte to traverse the binary file
    const uint8_t *buf_end = sec->content + sec->size;                      // pointer to end of binary module
    uint32_t mem_count;                                                            // auxiliary variable
    
    DEC_UINT32_LEB128 wasm_u32;                                                     // auxiliary variable to decode leb128 values
    uint8_t byte_val;
    
    Mem *mems;    
    Limits lim;

    //get mem count
    wasm_u32 = DecodeLeb128UInt32(index);
    if (wasm_u32.len == 0){
        result.id = WP_DIAG_ID_INVALID_SECTION_ID;     //TODO better error for decoding import section
        return result;                                        
    }

    index = index + wasm_u32.len;

    module->mem_len = wasm_u32.value;      // type's counter
    mem_count = wasm_u32.value;            // type's counter
   
    //alocating types on heap
    mems = ALLOCATE(Mem, mem_count);
    module->mems = mems;
    //TODO Check allocation works    
        
    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    for(uint32_t i = 0; i < mem_count; i++){        
        
        
        mems[i].usage = 0;
        index = DecodedLimitsType(index, &mems[i].lim);

        if(index == NULL){
             //Error
            //Decode fails
            result.id = WP_DIAG_ID_INVALID_SECTION_ID;     //TODO better error for decoding import section
            result.detail.place = 421;
            return result; 
        }
        

    }
    
    if(index != buf_end){
        //Decode fails
        result.id = WP_DIAG_ID_INVALID_SECTION_ID;     //TODO better error for decoding import section
        result.detail.place = 300;
        return result; 
    }

    return result; 
    
    #undef READ_BYTE
    #undef NOT_END
}

static WpError DecodeGlobalSection(WasmBinSection *sec, WasmModule *module){

    WpError result = CreateError(WP_DIAG_ID_OK, W_DIAG_MOD_LIST_DECODER, 22, 0);       //No error
    
    const uint8_t *index = sec->content;                       // pointer to byte to traverse the binary file
    const uint8_t *buf_end = sec->content + sec->size;                   // pointer to end of binary module
    uint32_t global_count;                                            // auxiliary variable
    
    DEC_UINT32_LEB128 wasm_u32;                                     // auxiliary variable to decode leb128 values
    uint8_t byte_val;
    EncodedValTypes encoded_type;

    Global *globals;    

    //get type count
    wasm_u32 = DecodeLeb128UInt32(index);
    if (wasm_u32.len == 0){
        result.id = WP_DIAG_ID_INVALID_SECTION_ID;     //TODO better error for decoding import section
        return result;                                        
    }

    index = index + wasm_u32.len;

    module->global_len = wasm_u32.value;      // type's counter
    global_count = wasm_u32.value;            // type's counter
   
    //alocating types on heap
    globals = ALLOCATE(Global, global_count);
    module->globals = globals;
    //TODO Check allocation works
    
    
    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    for(uint32_t i = 0; i < global_count; i++){

        //get type
        encoded_type = READ_BYTE();
        //TODO check valid type
        globals[i].type = encoded_type;
        
        //get mutable property
        byte_val = READ_BYTE();
        if(byte_val == 0){
            globals[i].mut = byte_val;
        }
        else if (byte_val == 1){
            globals[i].mut = byte_val;
        }
        else{
            result.id = WP_DIAG_ID_INVALID_SECTION_ID;     //TODO better error for decoding import section
            return result; 
        }
        

        globals[i].init_code = index;
        //look for opcode 0x0B;
        byte_val = READ_BYTE();
        while(NOT_END() && byte_val != OPCODE_END){
            byte_val = READ_BYTE();
        }
        globals[i].init_len = index - globals[1].init_code;          
        
    }
    
    if(index != buf_end){
        //Decode fails
        result.id = WP_DIAG_ID_INVALID_SECTION_ID;     //TODO better error for decoding import section
        result.detail.place = 114;
        return result; 
    }

    return result; 
    
    #undef READ_BYTE
    #undef NOT_END
}

static WpError DecodeExportSection(WasmBinSection *sec, WasmModule *module){

    WpError result = CreateError(WP_DIAG_ID_OK, W_DIAG_MOD_LIST_DECODER, 133, 0);       //No error
    
    const uint8_t *index = sec->content;                      // pointer to byte to traverse the binary file
    const uint8_t *buf_end = sec->content + sec->size;                 // pointer to end of binary module
    uint32_t export_count;
    DEC_UINT32_LEB128 wasm_u32;                                     // auxiliary variable to decode leb128 values
    uint8_t byte_val;

    WasmModuleExport *exports;    


    //get inports count
    wasm_u32 = DecodeLeb128UInt32(index);
    if (wasm_u32.len == 0){
        result.id = WP_DIAG_ID_INVALID_SECTION_ID;     //TODO better error for decoding import section
        return result;                                        
    }

    index = index + wasm_u32.len;
    module->exports_len = wasm_u32.value;   // import's counter
    export_count = wasm_u32.value;          // import's counter
   
    //alocating imports on heap
    exports = ALLOCATE(WasmModuleExport, export_count);
    module->exports = exports;
    //TODO Check allocation works

    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    for(uint32_t i = 0; i < export_count; i++){
        
        //get name's vector
        wasm_u32 = DecodeLeb128UInt32(index);
        if (wasm_u32.len == 0){
            result.id = WP_DIAG_ID_INVALID_SECTION_ID;     //TODO better error for decoding import section
            return result;                                        
        }
        index = index + wasm_u32.len;
        exports[i].name_len = wasm_u32.value;
        exports[i].name = index;

        //moving index to import desc
        index = index + wasm_u32.value;

        //Get export type
        exports[i].desc = ALLOCATE(ExportDesc, 1);
        //TODO Check allocation works

        byte_val = READ_BYTE();
        switch (byte_val){
            case 0:                
                exports[i].desc->type = WP_WAS_STRUC_MOD_EXPORT_DESC_TYPE_FUNC;
                break;
            case 1:
                exports[i].desc->type = WP_WAS_STRUC_MOD_EXPORT_DESC_TYPE_TABLE;
                break;
            case 2:
                exports[i].desc->type = WP_WAS_STRUC_MOD_EXPORT_DESC_TYPE_MEM;
                break;
            case 3:
                exports[i].desc->type = WP_WAS_STRUC_MOD_EXPORT_DESC_TYPE_GLOBAL;
                break;
            default:
                result.id = WP_DIAG_ID_INVALID_SECTION_ID;     //TODO better error for decoding export section
                return result; 
        }

        //get desc index
        wasm_u32 = DecodeLeb128UInt32(index);
         if (wasm_u32.len == 0){
            result.id = WP_DIAG_ID_INVALID_SECTION_ID;     //TODO better error for decoding import section
            return result;                                        
        }
        exports[i].desc->idx = wasm_u32.value;        

        //moving index to next import
        index = index + wasm_u32.len;

    }
    
    if(index != buf_end){
        //Decode fails
        result.id = WP_DIAG_ID_INVALID_SECTION_ID;     //TODO better error for decoding import section
        result.detail.place = 230;
        return result; 
    }

    return result; 
    
    #undef READ_BYTE
    #undef NOT_END
}

static WpError DecodeStartSection(WasmBinSection *sec, WasmModule *module){

    WpError result = CreateError(WP_DIAG_ID_OK, W_DIAG_MOD_LIST_DECODER, 133, 0);       //No error
    
    const uint8_t *index = sec->content;                      // pointer to byte to traverse the binary file
    const uint8_t *buf_end = sec->content + sec->size;                 // pointer to end of binary module
    
    DEC_UINT32_LEB128 wasm_u32;                                     // auxiliary variable to decode leb128 values
    uint8_t byte_val;

    
    //get start index
    wasm_u32 = DecodeLeb128UInt32(index);
    if (wasm_u32.len == 0){
        result.id = WP_DIAG_ID_INVALID_SECTION_ID;     //TODO better error for decoding import section
        return result;                                        
    }

    index = index + wasm_u32.len;
    module->start_function_index = wasm_u32.value;   // import's counter
    
    
    
    if(index != buf_end){
        //Decode fails
        result.id = WP_DIAG_ID_INVALID_SECTION_ID;     //TODO better error for decoding import section
        result.detail.place = 230;
        return result; 
    }

    return result;     
    
}
//TODO
static WpError DecodeElementSection(WasmBinSection *sec, WasmModule *module){

    WpError result = CreateError(WP_DIAG_ID_OK, W_DIAG_MOD_LIST_DECODER, 22, 0);       //No error
    
    const uint8_t *index = sec->content;                       // pointer to byte to traverse the binary file
    const uint8_t *buf_end = sec->content + sec->size;                   // pointer to end of binary module
    uint32_t element_count;                                            // auxiliary variable    
    
    uint8_t byte_val;
    uint32_t encoded_type;

    WasmModuleElement *elements;    

    //get type count
    index = DecodeLeb128UInt32_Fast(index, &element_count);
    if (index == NULL){
        result.id = WP_DIAG_ID_INVALID_SECTION_ID;     //TODO better error for decoding import section
        return result;                                        
    }
    module->element_len = element_count;      // type's counter
   
   
    //alocating elements on heap
    elements = ALLOCATE(WasmModuleElement, element_count);
    module->elements = elements;
    //TODO Check allocation works
    
    
    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    for(uint32_t i = 0; i < element_count; i++){

        //get type        
        index = DecodeLeb128UInt32_Fast(index, &encoded_type);
        if (index == NULL){
            result.id = WP_DIAG_ID_INVALID_SECTION_ID;     //TODO better error for decoding import section
            return result;                                        
        }
        
        switch(encoded_type) {
            case 0:
                break;
            case 1:
                break;
            case 2:
                break;
            case 3:
                break;
            case 4:
                break;
            case 5:
                break;
            case 6:
                break;
            case 7:
                break;
            default:
                result.id = WP_DIAG_ID_INVALID_SECTION_ID;     //TODO better error for decoding import section
                return result; 
            
        }       
        
    }
    
    if(index != buf_end){
        //Decode fails
        result.id = WP_DIAG_ID_INVALID_SECTION_ID;     //TODO better error for decoding import section
        result.detail.place = 114;
        return result; 
    }

    return result; 
    
    #undef READ_BYTE
    #undef NOT_END
}

static const uint8_t * DecodeCode(const uint8_t *code, WasmModule *module, uint32_t func_idx){

    /// A local is defined as a number of item and the type for each item (Spec 5.5.13)
    typedef struct Locals {
        uint32_t n;
        uint8_t type;
    } Locals;

    Locals *local;
    const uint8_t *index = code;
    const uint8_t *buf_end;                 // pointer to end of binary module  
    uint32_t code_size;
    uint32_t local_len;    
    uint32_t local_totals;
    uint32_t local_total_size;
    uint32_t i;
    uint32_t ii;
    uint32_t iii=0;

    //get code segment size
    index = DecodeLeb128UInt32_Fast(index, &code_size);
    if (index == NULL){            
            return NULL;                                        
    }
    buf_end = code + code_size;

    //get local vectors len
    index = DecodeLeb128UInt32_Fast(index, &local_len);
    if (index == NULL){
        return NULL;                                       
    }

    //Alocating space for local variable
    local = ALLOCATE(Locals, local_len);    
    //TODO Check allocation works    
    
    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    for(i = 0; i < local_len; i++){        
        
        //get number of element
        index = DecodeLeb128UInt32_Fast(index, &local[i].n);
        if (index == NULL){
           return NULL;                                    
        }

        //get type
        local[i].type = READ_BYTE();

        local_totals = local_totals + local[i].n;       //calc totals of local for field local_types_len
        
    }

    //filling local relate field for module's func ////////////////////////////////////////////////////////////////////
    module->funcs[func_idx].local_len = local_totals;
    module->funcs[func_idx].local_types = ALLOCATE(uint8_t, local_totals);
    //TODO check that allock works

    //filling type array
    for(i=0; i < local_len; i++){
        for(ii=0; ii < local[i].n; ii++){
            module->funcs[func_idx].local_types[iii]=local[i].type;
            iii++;
        }
    }
     /////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    //calc func body
    module->funcs[func_idx].body_len = buf_end - index;
    module->funcs[func_idx].body = index;
    
    if(*buf_end != OPCODE_END){
        //Decode fails instruction must be end with 0x0B        
        return NULL; 
    }

    return buf_end; 
    
    #undef READ_BYTE
    #undef NOT_END    
    
}

static WpError DecodeCodeSection(WasmBinSection *sec, WasmModule *module){

    WpError result = CreateError(WP_DIAG_ID_OK, W_DIAG_MOD_LIST_DECODER, 133, 0);       //No error
    
    const uint8_t *index = sec->content;                                // pointer to byte to traverse the binary file
    const uint8_t *buf_end = sec->content + sec->size;                  // pointer to end of binary module    
    uint32_t code_count;      
    

    //get code count
    index = DecodeLeb128UInt32_Fast(index, &code_count);
    if (index == NULL){
        result.id = WP_DIAG_ID_INVALID_SECTION_ID;     //TODO better error for decoding import section
        return result;                                        
    }
    
    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    for(uint32_t i = 0; i < code_count; i++){
        
        //call decode code segment
        if (i < module->func_len){
            //check that function exits
            index = DecodeCode(index, module, i);
        }
        else{
            result.id = WP_DIAG_ID_INVALID_SECTION_ID;     //TODO better error for decoding import section
            return result;  
        }
        

    }
    
    if(index != buf_end){
        //Decode fails
        result.id = WP_DIAG_ID_INVALID_SECTION_ID;     //TODO better error for decoding import section
        result.detail.place = 230;
        return result; 
    }

    return result; 
    
    #undef READ_BYTE
    #undef NOT_END    
    
}


WpError DecodeWasmBinModule(RuntimeEnv *self, WasmBinModule *bin_mod, WasmModule *mod){

    WpError result = CreateError(WP_DIAG_ID_OK, W_DIAG_MOD_LIST_DECODER, 253, 0);       //No error
    
    // If type sec is present:
    if(bin_mod->typesec.size > 0){
        result = DecodeTypeSection(&bin_mod->typesec, mod);        
    }
    
    // If import sec is present:
    if(bin_mod->importsec.size > 0){
        result = DecodeImportSection(&bin_mod->importsec, mod);
    }

    // If function sec is present:
    if(bin_mod->functionsec.size > 0){
        result = DecodeFunctionSection(&bin_mod->functionsec, mod);
    }

    // If table sec is present:
    if(bin_mod->tablesec.size > 0){
        result = DecodeTableSection(&bin_mod->tablesec, mod);
    }

    // If mem sec is present:
    if(bin_mod->memsec.size > 0){
        result = DecodeMemSection(&bin_mod->memsec, mod);
    }

    // If global sec is present:
    if(bin_mod->globalsec.size > 0){
        result = DecodeGlobalSection(&bin_mod->globalsec, mod);
    }

    // If export sec is present:
    if(bin_mod->exportsec.size > 0){
        result = DecodeExportSection(&bin_mod->exportsec, mod);
    }

    // If start sec is present:
    if(bin_mod->startsec.size > 0){
        result = DecodeStartSection(&bin_mod->startsec, mod);
    }

    // If element sec is present:
    if(bin_mod->elemsec.size > 0){
        result = DecodeElementSection(&bin_mod->elemsec, mod);
    }

    // If code sec is present:
    if(bin_mod->codesec.size > 0){
        result = DecodeCodeSection(&bin_mod->codesec, mod);
    }

    return result;

}