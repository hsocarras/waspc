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


#include "decoder/wasm_loader.h"
#include "memory/memory.h"
#include "utils/leb128.h"
#include "webassembly/binary/instructions.h"

#include <stdint.h>


/**
 * @brief  Function to decode expr rule
 * expr ::= (in:instr)* 0x0B ⇒ in* end
 * 
 * @param buf segment for a binary web asembly module
 * @param max_len max len to traverse the buffer
 * @param size Reference to a variable to store the expresion lenght in bytes.
 * @return const uint8_t* pointer to next byte after expr
 */
static const uint8_t * DecodeExpr(const uint8_t *const buf, const uint32_t max_len){

    const uint8_t *index = buf; 
    const uint8_t *buf_end = buf + max_len; 
    uint8_t byte_val;

    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    //look for opcode 0x0B;
    byte_val = READ_BYTE();
    while(NOT_END() && byte_val != OPCODE_END){
        byte_val = READ_BYTE();
    }

    if(*(index-1) == OPCODE_END ){        
        return index;
    }
    else{        
        return NULL;
    }

    #undef READ_BYTE
    #undef NOT_END
}

/**
 * @brief 
 * 
 * @param buf  Binary encoded wasm module
 * @param lim Limit type value
 * @return const uint8_t* return pointer to next byte after encoded limit.
 */
static const uint8_t * DecodedLimitsType(const uint8_t *const buf, Limits *lim){

    const uint8_t *index = buf; 
    uint32_t aux_u32;                                                        
    uint8_t byte_val;
    
    #define READ_BYTE() (*index++)

    //get index flag for maximun
    byte_val = READ_BYTE();
    index = DecodeLeb128UInt32(index, &aux_u32);           //get minimun value
    if (!index){
        lim->min = 0;
        lim->max = 0;
        return NULL;                                        
    }
    if(byte_val == 0){
        //maximun not present        
        
        lim->min = aux_u32;
        lim->max = 0;        
        return index;
    }
    else if (byte_val == 1){
        //maximun present
        
        lim->min = aux_u32;
        
        index = DecodeLeb128UInt32(index, &aux_u32);           //get maximun value
        if (!index){
            lim->max= 0;
            return NULL;                                        
        }
        
        lim->max = aux_u32;        
        return index;
    }
    else{
        //error encoded limit
        lim->min = 0;
        lim->max = 0;
        return NULL;
    }

    #undef READ_BYTE

}

/**
 * @brief function to decode vector of locals inside a code entry
 * 
 * @param code A binary segment for a encoded vector of locals non terminal symbol.
 *              locals ::= 𝑛:u32 𝑡:valtype
 * @param buf_end limit to avoid endless loop
 * @param func A function as Structure webassembly spec. See webassembly/structure/module.h
 * @return const uint8_t* pointer to next byte after vector if success, otherwise NULL.
 */
static const uint8_t * DecodeCodeLocals(const uint8_t *const code, const uint8_t *buf_end, Func *func){

    typedef struct EncodedLocal {
        uint32_t count;
        uint8_t val_type;
    }EncodedLocal;

    const uint8_t *index = code;    
    uint32_t local_count;
    uint32_t local_total = 0;
    uint32_t i = 0;
    uint32_t ii = 0;
    uint32_t iii = 0;
    EncodedLocal *vector_local;

    //get local count
    index = DecodeLeb128UInt32(index, &local_count);
    if (!index){
        return NULL;                                        
    }

    vector_local = ALLOCATE(EncodedLocal, local_count);
    if (!vector_local){        
        return NULL;                                       
    }

    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    for(i = 0; i < local_count; i++){        
        
        //get number of element
        index = DecodeLeb128UInt32(index, &vector_local[i].count);
        if (!index){        
            return NULL;                                     
        }

        if(NOT_END()){
            //get type
            vector_local[i].val_type = READ_BYTE();
            //Check val type
            switch(vector_local[i].val_type){
                case 0x7F:  //i32
                    break;
                case 0x7E:  //i64
                    break;
                case 0x7D:  //f32
                    break;
                case 0x7C:  //f64
                    break;
                case 0x7B:  //v128
                    break;
                case 0x70:  //funcref
                    break;
                case 0x6F:  //externref
                    break;
                default:
                    //error
                    return NULL;
            }

            local_total = local_total + vector_local[i].count;       //calc totals of local for field local_types_len
        }
        else{
            return NULL;
        }
        
    }

    //allocating space for function locals
    func->locals = ALLOCATE(uint8_t, local_total);
    if(!func->locals){
        return NULL;
    }
    func->local_len = local_total;

    //traversing the local_vector array to fill func.local
    for(i = 0; i < local_count; i++){        
        for(ii = 0; ii < vector_local[i].count; ii++){
            //check that index iii used for func.local is in range            
            if(iii < local_total){
                func->locals[iii] = vector_local[i].val_type;        //asign the value type 
                iii++;                                              //inc index
            }
            else{
                return NULL;
            }
        }        
    }

    //free vector_local;
    FREE_MEM(vector_local);

    return index;

    #undef READ_BYTE
    #undef NOT_END
}

/**
 * @brief function to decode a code entry inside code section
 * 
 * @param code A binary segment for a encoded code non terminal symbol.
 *              code ::= size:u32 code:func  
 * @param func A function as Structure webassembly spec. See webassembly/structure/module.h
 * @return const uint8_t* pointer to next byte after code entry if success, otherwise NULL.
 */
static const uint8_t * DecodeCode(const uint8_t *const code, Func *func){
        
    const uint8_t *index = code;
    const uint8_t *buf_end;                 // pointer to end of binary module  
    uint32_t code_size;    
    uint32_t i;
    uint32_t ii;
    uint32_t iii=0;
    
    //get code segment size
    index = DecodeLeb128UInt32(index, &code_size);    
    if (!index){  
        #if WASPC_CONFIG_DEV_FLAG == 1
        //TODO
        #endif
        return NULL;                                        
    } 
    buf_end = code + code_size; 
    
    //Decoding locals
    index = DecodeCodeLocals(index, buf_end, func);
    if (!index){  
        #if WASPC_CONFIG_DEV_FLAG == 1
        //TODO
        #endif
        return NULL;                                        
    }
    
    //calc func body
    func->body_len = buf_end - index;
    func->body = index;
    
    //Last byte code must be END
    if(*buf_end != OPCODE_END){
        //Decode fails instruction must be end with 0x0B        
        return NULL; 
        #if WASPC_CONFIG_DEV_FLAG == 1
        //TODO
        #endif
        return NULL;  
    }
    buf_end++;          //pointter to next byte for next code segment    
    return buf_end;   
    
}

/**
 * @brief Decode Type Section
 * 
 * @param sec Binary section
 * @param mod pointer to decoded mod instance.
 * @return WpResult error or types length if succsess in u32.
 */
WpResult DecodeTypeSection(const WasmBinSection *const sec, WpDecodedModule *mod){

    WpResult result;
    WpResultInit(&result);
    FuncType *types = NULL;
    const uint8_t *index = sec->content;                        // pointer to byte to traverse the binary file
    const uint8_t *buf_end = sec->content + sec->size;          // pointer to end of binary module
    uint32_t type_count;                                        // auxiliary variable
    uint32_t param_count;
    uint32_t aux_u32;                                           // auxiliary variable to decode leb128 values
    uint8_t byte_val;
    ValType encoded_type;

    
    //get type count
    index = DecodeLeb128UInt32(index, &type_count);
    if (!index){        
        //invalid LEB128 encoded
        WpResultAddError(&result, WP_DIAG_ID_DECODE_LEB128_FAIL, W_DIAG_MOD_LIST_DECODER);
        #if WASPC_CONFIG_DEV_FLAG == 1
        //TODO
        #endif
        return result;                                        
    }  
    
    //alocating types on heap
    types = ALLOCATE(FuncType, type_count);    
    if (!types){             
        //allocation check
        WpResultAddError(&result, WP_DIAG_ID_ALLOCATION_FAILURE, W_DIAG_MOD_LIST_DECODER);
        #if WASPC_CONFIG_DEV_FLAG == 1
        //TODO
        #endif
        return result;                                        
    }  

    //TODO maybe is good idea make something similar to string internals (Crafting interpreter's book)
    // for function signature.
    
    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    for(uint32_t i = 0; i < type_count; i++){

        //get functype (0x60)
        encoded_type = READ_BYTE();
        if (encoded_type != WAS_FUNCT_TYPE){
            //invalid encoded
            WpResultAddError(&result, WP_DIAG_ID_INVALID_ENCODED_VALUE, W_DIAG_MOD_LIST_DECODER);
            #if WASPC_CONFIG_DEV_FLAG == 1
            //TODO
            #endif
            return result;                                        
        }
        
        //get parameters count        
        index = DecodeLeb128UInt32(index, &aux_u32);
        if (!index){
            //invalid encoded
            WpResultAddError(&result, WP_DIAG_ID_DECODE_LEB128_FAIL, W_DIAG_MOD_LIST_DECODER);
            #if WASPC_CONFIG_DEV_FLAG == 1
            //TODO
            #endif
            return result;                                      
        }
        
        param_count = aux_u32;
        types[i].param.len = param_count;

        //allocating array for param count        
        types[i].param.types = ALLOCATE(ValType, param_count);          
        if (!types[i].param.types){        
            //allocation check
            WpResultAddError(&result, WP_DIAG_ID_ALLOCATION_FAILURE, W_DIAG_MOD_LIST_DECODER);
            #if WASPC_CONFIG_DEV_FLAG == 1
            //TODO
            #endif
            return result;                                       
        } 

        //Loop for read parameter vector
        for(uint32_t ii = 0; ii < param_count; ii++){

            //get encoded parameter type            
            encoded_type = READ_BYTE();

            //TODO check for valid type
            types[i].param.types[ii] = encoded_type;
        }
        
        //getting result count
        index = DecodeLeb128UInt32(index, &aux_u32);
        if (!index){
           //invalid LEB128 encoded
            WpResultAddError(&result, WP_DIAG_ID_DECODE_LEB128_FAIL, W_DIAG_MOD_LIST_DECODER);
            #if WASPC_CONFIG_DEV_FLAG == 1
            //TODO
            #endif
            return result;                                         
        }
        
        param_count = aux_u32;
        types[i].ret.len = param_count;
        
        //allocating array for result count
        types[i].ret.types = ALLOCATE(ValType, param_count);
        if (!types[i].ret.types){        
            //allocation check
            WpResultAddError(&result, WP_DIAG_ID_ALLOCATION_FAILURE, W_DIAG_MOD_LIST_DECODER);
            #if WASPC_CONFIG_DEV_FLAG == 1
            //TODO
            #endif
            return result;                                       
        }

        //Loop for read result vector
        for(uint32_t ii = 0; ii < param_count; ii++){

            //get encoded parameter type            
            encoded_type = READ_BYTE();            
            //TODO check for valid type
            types[i].ret.types[ii] = encoded_type;
            
        }  
          
        
    }
    
    if(index != buf_end){
        //section check
        WpResultAddError(&result, WP_DIAG_ID_ERROR_DECODE_TYPE_SECTION, W_DIAG_MOD_LIST_DECODER);
        #if WASPC_CONFIG_DEV_FLAG == 1
        //TODO
        #endif
        return result; 
    }
    mod->types = types;
    mod->type_len = type_count;
    result.value.u32 = type_count;
    return result; 
    
    #undef READ_BYTE
    #undef NOT_END
}

/**
 * @brief function to decode the import section in a binary webassembly module
 * 
 * @param sec Binary section
 * @param mod pointer to decoded mod instance.
 * @return WpResult error or import arraay's length if succsess in u32.
 */
WpResult DecodeImportSection(const WasmBinSection *const sec, WpDecodedModule *mod){

    WpResult result;
    WpResultInit(&result);
    Import *imports;
    const uint8_t *index = sec->content;                        // pointer to byte to traverse the binary file
    const uint8_t *buf_end = sec->content + sec->size;                 // pointer to end of binary module
    uint32_t import_count;
    uint32_t aux_u32;                                           // auxiliary variable to decode leb128 values
    uint8_t byte_val;

    
    //get inports count
    index = DecodeLeb128UInt32(index, &import_count);
    if (!index){
        //invalid LEB128 encoded
        WpResultAddError(&result, WP_DIAG_ID_DECODE_LEB128_FAIL, W_DIAG_MOD_LIST_DECODER);
        #if WASPC_CONFIG_DEV_FLAG == 1
        //TODO
        #endif
        return result;                                       
    }
       
    //alocating imports on heap
    imports = ALLOCATE(Import, import_count);
    if (!imports){        
        //allocation check
        WpResultAddError(&result, WP_DIAG_ID_ALLOCATION_FAILURE, W_DIAG_MOD_LIST_DECODER);
        #if WASPC_CONFIG_DEV_FLAG == 1
        //TODO
        #endif
        return result;                                       
    }
       
    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    for(uint32_t i = 0; i < import_count; i++){
        //get module name's vector
        index = DecodeLeb128UInt32(index, &aux_u32);
        if (!index){
           //invalid LEB128 encoded
            WpResultAddError(&result, WP_DIAG_ID_DECODE_LEB128_FAIL, W_DIAG_MOD_LIST_DECODER);
            #if WASPC_CONFIG_DEV_FLAG == 1
            //TODO
            #endif
            return result;                                        
        }
        
        imports[i].module_len = aux_u32;
        imports[i].module = index;

        //moving index to name vector
        index = index + aux_u32;

        //get name's vector
        index = DecodeLeb128UInt32(index, &aux_u32);
        if (!index){
            //invalid LEB128 encoded
            WpResultAddError(&result, WP_DIAG_ID_DECODE_LEB128_FAIL, W_DIAG_MOD_LIST_DECODER);
            #if WASPC_CONFIG_DEV_FLAG == 1
            //TODO
            #endif
            return result;                                        
        }
        
        imports[i].name_len = aux_u32;
        imports[i].name = index;

        //moving index to import desc
        index = index + aux_u32;

        //Get import type
        imports[i].desc = ALLOCATE(ImportDesc, 1);
        if (!imports[i].desc){        
            //allocation check
            WpResultAddError(&result, WP_DIAG_ID_ALLOCATION_FAILURE, W_DIAG_MOD_LIST_DECODER);
            #if WASPC_CONFIG_DEV_FLAG == 1
            //TODO
            #endif
            return result;                                       
        }
        

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
                //invalid desc type
                WpResultAddError(&result, WP_DIAG_ID_INVALID_IMPORT_DESC_TYPE, W_DIAG_MOD_LIST_DECODER);
                 #if WASPC_CONFIG_DEV_FLAG == 1
                //TODO
                #endif 
                return result; 
        }

        //get desc index
        index = DecodeLeb128UInt32(index, &aux_u32);
        if (!index){
            //invalid LEB128 encoded
            WpResultAddError(&result, WP_DIAG_ID_DECODE_LEB128_FAIL, W_DIAG_MOD_LIST_DECODER);
            #if WASPC_CONFIG_DEV_FLAG == 1
            //TODO
            #endif
            return result;                                        
        }
        imports[i].desc->idx = aux_u32;        


    }
    
    if(index != buf_end){
        //section check
        WpResultAddError(&result, WP_DIAG_ID_ERROR_DECODE_IMPORT_SECTION, W_DIAG_MOD_LIST_DECODER);
        #if WASPC_CONFIG_DEV_FLAG == 1
        //TODO
        #endif
        return result;
    }
    
    mod->imports = imports;
    mod->imports_len = import_count;
    result.value.u32 = import_count;
    return result; 
    
    #undef READ_BYTE
    #undef NOT_END
}

/**
 * @brief function to decode the function section in a binary webassembly module.
 * 
 * @param sec Binary section.
 * @param mod pointer to decoded mod instance.
 * @return WpResult error or function array's length if succsess in u32.
 */
WpResult DecodeFunctionSection(const WasmBinSection *const sec, WpDecodedModule *mod){

    WpResult result;
    WpResultInit(&result);
    Func *funcs;
    const uint8_t *index = sec->content;                                        // pointer to byte to traverse the binary file
    const uint8_t *buf_end = sec->content + sec->size;                          // pointer to end of binary module
    uint32_t func_count;                                                        // auxiliary variable    
    uint32_t aux_u32;                                                           // auxiliary variable to decode leb128 values
    uint8_t byte_val;
    ValType encoded_type;
    

    //get functions count
    index = DecodeLeb128UInt32(index, &func_count);
    if (!index){
        //invalid LEB128 encoded
        WpResultAddError(&result, WP_DIAG_ID_DECODE_LEB128_FAIL, W_DIAG_MOD_LIST_DECODER);
        #if WASPC_CONFIG_DEV_FLAG == 1
        //TODO
        #endif
        return result;                                       
    }
   
    //alocating types on heap
    funcs = ALLOCATE(Func, func_count);
    if (!funcs){        
        //allocation check
        WpResultAddError(&result, WP_DIAG_ID_ALLOCATION_FAILURE, W_DIAG_MOD_LIST_DECODER);
        #if WASPC_CONFIG_DEV_FLAG == 1
        //TODO
        #endif
        return result;                                       
    }    
      
    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    for(uint32_t i = 0; i < func_count; i++){

        
        
        //get parameters count        
        index = DecodeLeb128UInt32(index, &aux_u32);
        if (!index){
            //invalid LEB128 encoded
            WpResultAddError(&result, WP_DIAG_ID_DECODE_LEB128_FAIL, W_DIAG_MOD_LIST_DECODER);
            #if WASPC_CONFIG_DEV_FLAG == 1
            //TODO
            #endif
            return result;                                       
        }
               
        funcs[i].type = aux_u32;
        funcs[i].local_len = 0;
        funcs[i].locals = NULL;
        funcs[i].body_len = 0;
        funcs[i].body = NULL;

    }
    
    if(index != buf_end){
        //section check
        WpResultAddError(&result, WP_DIAG_ID_ERROR_DECODE_FUNCTION_SECTION, W_DIAG_MOD_LIST_DECODER);
        #if WASPC_CONFIG_DEV_FLAG == 1
        //TODO
        #endif
        return result; 
    }

    mod->funcs = funcs;
    mod->func_len = func_count;
    result.value.u32 = func_count;
    return result; 
    
    #undef READ_BYTE
    #undef NOT_END
}

/**
 * @brief function to decode the table section in a binary webassembly module.
 * 
 * @param sec Binary section.
 * @param mod pointer to decoded mod instance.
 * @return WpResult error or tabletype array's length if succsess in u32.
 */
WpResult DecodeTableSection(const WasmBinSection *const sec, WpDecodedModule *mod){

    WpResult result;
    WpResultInit(&result);
    TableType *tables;
    const uint8_t *index = sec->content;                                        // pointer to byte to traverse the binary file
    const uint8_t *buf_end = sec->content + sec->size;                          // pointer to end of binary module
    uint32_t table_count;                                                       // auxiliary variable
    
    uint32_t aux_u32;                                                           // auxiliary variable to decode leb128 values
    uint8_t byte_val;
    Limits lim;

    //get table count
    index = DecodeLeb128UInt32(index, &table_count);
    if (!index){
        //invalid LEB128 encoded
        WpResultAddError(&result, WP_DIAG_ID_DECODE_LEB128_FAIL, W_DIAG_MOD_LIST_DECODER);
        #if WASPC_CONFIG_DEV_FLAG == 1
        //TODO
        #endif
        return result;                                       
    }
       
    //alocating types on heap
    tables = ALLOCATE(TableType, table_count);
    if (!tables){        
        //allocation check
        WpResultAddError(&result, WP_DIAG_ID_ALLOCATION_FAILURE, W_DIAG_MOD_LIST_DECODER);
        #if WASPC_CONFIG_DEV_FLAG == 1
        //TODO
        #endif
        return result;                                       
    }   
        
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
            WpResultAddError(&result, WP_DIAG_ID_INVALID_REFERENCE_TYPE, W_DIAG_MOD_LIST_DECODER);
            #if WASPC_CONFIG_DEV_FLAG == 1
            //TODO
            #endif
            return result; 
        }
        
        index = DecodedLimitsType(index, &tables[i].lim);
        if(index == NULL){             
            //Decode fails
            WpResultAddError(&result, WP_DIAG_ID_INVALID_LIMITS_TYPE, W_DIAG_MOD_LIST_DECODER);
            #if WASPC_CONFIG_DEV_FLAG == 1
            //TODO
            #endif
            return result;  
        }   
    }
    
    if(index != buf_end){
        //Decode fails
        WpResultAddError(&result, WP_DIAG_ID_ERROR_DECODE_TABLE_SECTION, W_DIAG_MOD_LIST_DECODER);
        #if WASPC_CONFIG_DEV_FLAG == 1
        //TODO
        #endif
        return result; 
    }
    mod->tables = tables;
    mod->table_len = table_count;
    result.value.u32 = table_count;
    return result; 
    
    #undef READ_BYTE
    #undef NOT_END
}

/**
 * @brief function to decode the mem section in a binary webassembly module.
 * 
 * @param sec Binary section.
 * @param mod pointer to decoded mod instance.
 * @return WpResult error or memtype array's length if succsess in u32.
 */
WpResult DecodeMemSection( const WasmBinSection *const sec, WpDecodedModule *mod){

    WpResult result;
    WpResultInit(&result);
    MemType *mems;
    const uint8_t *index = sec->content;                                        // pointer to byte to traverse the binary file
    const uint8_t *buf_end = sec->content + sec->size;                      // pointer to end of binary module
    uint32_t mem_count;                                                            // auxiliary variable    
    uint32_t aux_u32;                                                       // auxiliary variable to decode leb128 values
    uint8_t byte_val;
    
       
    Limits lim;

    //get mem count
    index = DecodeLeb128UInt32(index, &mem_count);
    if (!index){
        //invalid LEB128 encoded
        WpResultAddError(&result, WP_DIAG_ID_DECODE_LEB128_FAIL, W_DIAG_MOD_LIST_DECODER);
        #if WASPC_CONFIG_DEV_FLAG == 1
        //TODO
        #endif
        return result;                                       
    }
   
    //alocating types on heap
    mems = ALLOCATE(MemType, mem_count);
    if (!mems){        
        //allocation check
        WpResultAddError(&result, WP_DIAG_ID_ALLOCATION_FAILURE, W_DIAG_MOD_LIST_DECODER);
        #if WASPC_CONFIG_DEV_FLAG == 1
        //TODO
        #endif
        return result;                                       
    }  
        
    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    for(uint32_t i = 0; i < mem_count; i++){       
        
        
        index = DecodedLimitsType(index, &mems[i]);

        if(index == NULL){             
            //Decode fails
            WpResultAddError(&result, WP_DIAG_ID_INVALID_LIMITS_TYPE, W_DIAG_MOD_LIST_DECODER);
            #if WASPC_CONFIG_DEV_FLAG == 1
            //TODO
            #endif
            return result;  
        }
        

    }
    
    if(index != buf_end){
        //Decode fails
        WpResultAddError(&result, WP_DIAG_ID_ERROR_DECODE_MEM_SECTION, W_DIAG_MOD_LIST_DECODER);
        #if WASPC_CONFIG_DEV_FLAG == 1
        //TODO
        #endif
        return result; 
    }

    mod->mems = mems;
    mod->mem_len = mem_count;
    result.value.u32 = mem_count;
    return result; 
    
    #undef READ_BYTE
    #undef NOT_END
}

/**
 * @brief function to decode the global section in a binary webassembly module.
 * 
 * @param sec Binary section.
 * @param mod pointer to decoded mod instance.
 * @return WpResult error or global array's length if succsess in u32.
 */
WpResult DecodeGlobalSection(const WasmBinSection *const sec, WpDecodedModule *mod){

    WpResult result;
    WpResultInit(&result);
    Global *globals;
    const uint8_t *index = sec->content;                        // pointer to byte to traverse the binary file
    const uint8_t *buf_end = sec->content + sec->size;                   // pointer to end of binary module
    uint32_t global_count;                                            
    uint32_t aux_u32;                                           // auxiliary variable
    uint8_t byte_val;
    ValType encoded_type;
        

    //get type count
    index = DecodeLeb128UInt32(index, &global_count);
    if (!index){
        //invalid LEB128 encoded
        WpResultAddError(&result, WP_DIAG_ID_DECODE_LEB128_FAIL, W_DIAG_MOD_LIST_DECODER);
        #if WASPC_CONFIG_DEV_FLAG == 1
        //TODO
        #endif
        return result;                                       
    }
   
    //alocating types on heap
    globals = ALLOCATE(Global, global_count);
    if (!globals){        
        //allocation check
        WpResultAddError(&result, WP_DIAG_ID_ALLOCATION_FAILURE, W_DIAG_MOD_LIST_DECODER);
        #if WASPC_CONFIG_DEV_FLAG == 1
        //TODO
        #endif
        return result;                                       
    } 
    
    
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
            WpResultAddError(&result, WP_DIAG_ID_INVALID_GLOBAL_TYPE, W_DIAG_MOD_LIST_DECODER);
            #if WASPC_CONFIG_DEV_FLAG == 1
            //TODO
            #endif
            return result; 
        }
        

        globals[i].expr = index;
        index = DecodeExpr(index, buf_end-index);        
        if (!index){
            //invalid LEB128 encoded
            WpResultAddError(&result, WP_DIAG_ID_DECODE_EXPR_FAIL, W_DIAG_MOD_LIST_DECODER);
            #if WASPC_CONFIG_DEV_FLAG == 1
            //TODO
            #endif
            return result;                                       
        }        
        globals[i].expr_len = index - globals[i].expr;
    }
    
    if(index != buf_end){
        //Decode fails
        WpResultAddError(&result, WP_DIAG_ID_ERROR_DECODE_GLOBAL_SECTION, W_DIAG_MOD_LIST_DECODER);
        #if WASPC_CONFIG_DEV_FLAG == 1
        //TODO
        #endif
        return result; 
    }

    mod->globals = globals;
    mod->global_len = global_count;
    result.value.u32 = global_count;
    return result; 
    
    #undef READ_BYTE
    #undef NOT_END
}

/**
 * @brief function to decode the export section on a binary webassembly module.
 * 
 * @param sec Binary section.
 * @param mod pointer to decoded mod instance.
 * @return WpResult error or export array's length if succsess in u32.
 */
WpResult DecodeExportSection(const WasmBinSection *const sec, WpDecodedModule *mod){

    WpResult result;
    WpResultInit(&result);
    Export *exports;
    const uint8_t *index = sec->content;                      // pointer to byte to traverse the binary file
    const uint8_t *buf_end = sec->content + sec->size;                 // pointer to end of binary module
    uint32_t export_count;
    uint32_t aux_u32;                                     // auxiliary variable to decode leb128 values
    uint8_t byte_val;
    
    //get inports count
    index = DecodeLeb128UInt32(index, &export_count);
    if (!index){
        //invalid LEB128 encoded
        WpResultAddError(&result, WP_DIAG_ID_DECODE_LEB128_FAIL, W_DIAG_MOD_LIST_DECODER);
        #if WASPC_CONFIG_DEV_FLAG == 1
        //TODO
        #endif
        return result;                                       
    }

    
    //alocating imports on heap
    exports = ALLOCATE(Export, export_count);
    if (!exports){        
        //allocation check
        WpResultAddError(&result, WP_DIAG_ID_ALLOCATION_FAILURE, W_DIAG_MOD_LIST_DECODER);
        #if WASPC_CONFIG_DEV_FLAG == 1
        //TODO
        #endif
        return result;                                       
    } 

    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    for(uint32_t i = 0; i < export_count; i++){
        
        //get name's vector
        index = DecodeLeb128UInt32(index, &aux_u32);
        if (!index){
            //invalid LEB128 encoded
            WpResultAddError(&result, WP_DIAG_ID_DECODE_LEB128_FAIL, W_DIAG_MOD_LIST_DECODER);
            #if WASPC_CONFIG_DEV_FLAG == 1
            //TODO
            #endif
            return result;                                       
        }        
        exports[i].name_len = aux_u32;
        exports[i].name = index;

        //moving index to import desc
        index = index + aux_u32;

        //Get export type
        exports[i].desc = ALLOCATE(ExportDesc, 1);
        if (!exports[i].desc){        
            //allocation check
            WpResultAddError(&result, WP_DIAG_ID_ALLOCATION_FAILURE, W_DIAG_MOD_LIST_DECODER);
            #if WASPC_CONFIG_DEV_FLAG == 1
            //TODO
            #endif
            return result;                                       
        }

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
                WpResultAddError(&result, WP_DIAG_ID_INVALID_EXPORT_DESCRIPTION_TYPE, W_DIAG_MOD_LIST_DECODER);
                #if WASPC_CONFIG_DEV_FLAG == 1
                //TODO
                #endif
                return result;
        }

        //get desc index
        index = DecodeLeb128UInt32(index, &aux_u32);
        if (!index){
            //invalid LEB128 encoded
            WpResultAddError(&result, WP_DIAG_ID_DECODE_LEB128_FAIL, W_DIAG_MOD_LIST_DECODER);
            #if WASPC_CONFIG_DEV_FLAG == 1
            //TODO
            #endif
            return result;                                       
        }
        exports[i].desc->idx = aux_u32;

    }
    
    if(index != buf_end){
        //Decode fails
        WpResultAddError(&result, WP_DIAG_ID_ERROR_DECODE_EXPORT_SECTION, W_DIAG_MOD_LIST_DECODER);
        #if WASPC_CONFIG_DEV_FLAG == 1
        //TODO
        #endif
        return result;
    }

    mod->exports = exports;
    mod->exports_len = export_count;
    result.value.u32 = export_count;
    return result; 
    
    #undef READ_BYTE
    #undef NOT_END
}

/**
 * @brief function to decode the start section on a binary webassembly module.
 * 
 * @param sec Binary section.
 * @param mod pointer to decoded mod instance.
 * @return WpResult error or start value if succsess in u32.
 */
WpResult DecodeStartSection(const WasmBinSection *const sec, WpDecodedModule *mod){

    WpResult result;
    WpResultInit(&result);
    uint32_t start;
    const uint8_t *index = sec->content;                      // pointer to byte to traverse the binary file
    const uint8_t *buf_end = sec->content + sec->size;                 // pointer to end of binary module    
  
    
    //get start index
    index = DecodeLeb128UInt32(index, &start);
    if (!index){
        //invalid LEB128 encoded
        WpResultAddError(&result, WP_DIAG_ID_DECODE_LEB128_FAIL, W_DIAG_MOD_LIST_DECODER);
        #if WASPC_CONFIG_DEV_FLAG == 1
        //TODO
        #endif
        return result;                                       
    }
    
    if(index != buf_end){
        //Decode fails
        WpResultAddError(&result, WP_DIAG_ID_ERROR_DECODE_START_SECTION, W_DIAG_MOD_LIST_DECODER);
        #if WASPC_CONFIG_DEV_FLAG == 1
        //TODO
        #endif
        return result;
    }
    
    mod->start = start;
    result.value.u32 = 1;
    return result;     
    
}

/**
 * @brief function to decode the element section on a binary webassembly module.
 * 
 * @param sec Binary section.
 * @param mod pointer to decoded mod instance.
 * @return WpResult error or elem array's length if succsess in u32.
 */
WpResult DecodeElementSection(const WasmBinSection *const sec, WpDecodedModule *mod){

    WpResult result;
    WpResultInit(&result);
    Elem *elems;
    const uint8_t *index = sec->content;                       // pointer to byte to traverse the binary file
    const uint8_t *buf_end = sec->content + sec->size;                   // pointer to end of binary module
    const uint8_t * start_pos;
    uint32_t element_count;                                            // auxiliary variable   
    uint32_t expr_len;     
    uint8_t byte_val;
    uint32_t encoded_type;

    

    //get type count
    index = DecodeLeb128UInt32(index, &element_count);
    if (!index){
        //invalid LEB128 encoded
        WpResultAddError(&result, WP_DIAG_ID_DECODE_LEB128_FAIL, W_DIAG_MOD_LIST_DECODER);
        #if WASPC_CONFIG_DEV_FLAG == 1
        //TODO
        #endif
        return result;                                       
    }   
   
    //alocating elements on heap
    elems = ALLOCATE(Elem, element_count);
    if (!elems){        
        //allocation check
        WpResultAddError(&result, WP_DIAG_ID_ALLOCATION_FAILURE, W_DIAG_MOD_LIST_DECODER);
        #if WASPC_CONFIG_DEV_FLAG == 1
        //TODO
        #endif
        return result;                                       
    } 
    
    
    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    for(uint32_t i = 0; i < element_count; i++){

        //get type        
        index = DecodeLeb128UInt32(index, &encoded_type);
        if (!index){
            //invalid LEB128 encoded
            WpResultAddError(&result, WP_DIAG_ID_DECODE_LEB128_FAIL, W_DIAG_MOD_LIST_DECODER);
            #if WASPC_CONFIG_DEV_FLAG == 1
            //TODO
            #endif
            return result;                                       
        }
        
        switch(encoded_type) {
            /// 0:u32 𝑒:expr 𝑦*:vec(funcidx) ⇒ {type funcref, init ((ref.func 𝑦) end)*, mode active {table 0, offset 𝑒}}
            case 0:
                elems[i].mode = WP_WAS_STRUC_MOD_ELEMENT_MODE_ACTIVE;
                elems[i].type = WAS_FUNC_REF_TYPE;
                elems[i].table_idx = 0;

                start_pos = index;
                index = DecodeExpr(index, sec->size);
                if (!index){
                    //invalid LEB128 encoded
                    WpResultAddError(&result, WP_DIAG_ID_DECODE_EXPR_FAIL, W_DIAG_MOD_LIST_DECODER);
                    #if WASPC_CONFIG_DEV_FLAG == 1
                    //TODO
                    #endif
                    return result;                                       
                }
                elems[i].offset_len = expr_len;
                elems[i].offset = start_pos;

                //get init vector len 
                index = DecodeLeb128UInt32(index, &expr_len);
                if (!index){
                    //invalid LEB128 encoded
                    WpResultAddError(&result, WP_DIAG_ID_DECODE_LEB128_FAIL, W_DIAG_MOD_LIST_DECODER);
                    #if WASPC_CONFIG_DEV_FLAG == 1
                    //TODO
                    #endif
                    return result;                                       
                }
                elems[i].init_len = expr_len;
                elems[i].init = index;
                 
                break;
            /// 1:u32 et : elemkind 𝑦*:vec(funcidx) ⇒ {type et, init ((ref.func 𝑦) end)*, mode passive}
            case 1:
                elems[i].mode = WP_WAS_STRUC_MOD_ELEMENT_MODE_PASSIVE;
                elems[i].type = READ_BYTE();
                //TODO Check  elemking must be 0x00

                elems[i].table_idx = 0;

                elems[i].offset_len = 0;
                elems[i].offset = NULL;

                index = DecodeLeb128UInt32(index, &expr_len);
                if (!index){
                    //invalid LEB128 encoded
                    WpResultAddError(&result, WP_DIAG_ID_DECODE_LEB128_FAIL, W_DIAG_MOD_LIST_DECODER);
                    #if WASPC_CONFIG_DEV_FLAG == 1
                    //TODO
                    #endif
                    return result;                                       
                }
                elems[i].init_len = expr_len;
                elems[i].init = index;
                break;
            //2:u32 𝑥:tableidx 𝑒:expr et : elemkind 𝑦*:vec(funcidx) ⇒ {type et, init ((ref.func 𝑦) end)*, mode active {table 𝑥, offset 𝑒}}
            case 2:

                elems[i].mode = WP_WAS_STRUC_MOD_ELEMENT_MODE_ACTIVE;

                index = DecodeLeb128UInt32(index, &expr_len);
                if (!index){
                    //invalid LEB128 encoded
                    WpResultAddError(&result, WP_DIAG_ID_DECODE_LEB128_FAIL, W_DIAG_MOD_LIST_DECODER);
                    #if WASPC_CONFIG_DEV_FLAG == 1
                    //TODO
                    #endif
                    return result;                                       
                }
                elems[i].table_idx = expr_len;

                start_pos = index;
                index = DecodeExpr(index, sec->size);
                if (!index){
                    //invalid LEB128 encoded
                    WpResultAddError(&result, WP_DIAG_ID_DECODE_EXPR_FAIL, W_DIAG_MOD_LIST_DECODER);
                    #if WASPC_CONFIG_DEV_FLAG == 1
                    //TODO
                    #endif
                    return result;                                       
                }
                elems[i].offset_len = expr_len;
                elems[i].offset = start_pos;

                elems[i].type = READ_BYTE();
                //TODO check elemkind is 0x00

                index = DecodeLeb128UInt32(index, &expr_len);
                if (!index){
                    //invalid LEB128 encoded
                    WpResultAddError(&result, WP_DIAG_ID_DECODE_LEB128_FAIL, W_DIAG_MOD_LIST_DECODER);
                    #if WASPC_CONFIG_DEV_FLAG == 1
                    //TODO
                    #endif
                    return result;                                       
                }
                elems[i].init_len = expr_len;
                elems[i].init = index;                
                
                break;
            // 3:u32 et : elemkind 𝑦*:vec(funcidx) ⇒ {type et, init ((ref.func 𝑦) end)*, mode declarative}
            case 3:

                elems[i].mode = WP_WAS_STRUC_MOD_ELEMENT_MODE_DECLARATIVE;
                elems[i].type = READ_BYTE();
                //TODO check elemking is 0x00

                elems[i].offset_len = 0;
                elems[i].offset = NULL;

                index = DecodeLeb128UInt32(index, &expr_len);
                if (!index){
                    //invalid LEB128 encoded
                    WpResultAddError(&result, WP_DIAG_ID_DECODE_LEB128_FAIL, W_DIAG_MOD_LIST_DECODER);
                    #if WASPC_CONFIG_DEV_FLAG == 1
                    //TODO
                    #endif
                    return result;                                       
                }
                elems[i].init_len = expr_len;
                elems[i].init = index;

                break;
            // 4:u32 𝑒:expr el*:vec(expr) ⇒ {type funcref, init el*, mode active {table 0, offset 𝑒}}
            case 4:
                elems[i].mode = WP_WAS_STRUC_MOD_ELEMENT_MODE_ACTIVE;
                elems[i].type = WAS_FUNC_REF_TYPE;
                elems[i].table_idx = 0;

                start_pos = index;
                index = DecodeExpr(index, sec->size);
                if (!index){
                    //invalid LEB128 encoded
                    WpResultAddError(&result, WP_DIAG_ID_DECODE_EXPR_FAIL, W_DIAG_MOD_LIST_DECODER);
                    #if WASPC_CONFIG_DEV_FLAG == 1
                    //TODO
                    #endif
                    return result;                                       
                }
                elems[i].offset_len = expr_len;
                elems[i].offset = start_pos;

                //get init vector len 
                index = DecodeLeb128UInt32(index, &expr_len);
                if (!index){
                    //invalid LEB128 encoded
                    WpResultAddError(&result, WP_DIAG_ID_DECODE_LEB128_FAIL, W_DIAG_MOD_LIST_DECODER);
                    #if WASPC_CONFIG_DEV_FLAG == 1
                    //TODO
                    #endif
                    return result;                                       
                }
                elems[i].init_len = expr_len;
                elems[i].init = index;

                break;
            // 5:u32 et : reftype el*:vec(expr) ⇒ {type 𝑒𝑡, init el*, mode passive}
            case 5:

                elems[i].mode = WP_WAS_STRUC_MOD_ELEMENT_MODE_PASSIVE;
                elems[i].type = WAS_EXTERN_REF_TYPE;
                elems[i].table_idx = 0;

                elems[i].offset_len = 0;
                elems[i].offset = NULL;

                //get init vector len 
                index = DecodeLeb128UInt32(index, &expr_len);
                if (!index){
                    //invalid LEB128 encoded
                    WpResultAddError(&result, WP_DIAG_ID_DECODE_LEB128_FAIL, W_DIAG_MOD_LIST_DECODER);
                    #if WASPC_CONFIG_DEV_FLAG == 1
                    //TODO
                    #endif
                    return result;                                       
                }
                elems[i].init_len = expr_len;
                elems[i].init = index;

                break;
            // 6:u32 𝑥:tableidx 𝑒:expr et : reftype el*:vec(expr) ⇒ {type 𝑒𝑡, init el*, mode active {table 𝑥, offset 𝑒}}
            case 6:

                elems[i].mode = WP_WAS_STRUC_MOD_ELEMENT_MODE_ACTIVE;
                elems[i].type = WAS_EXTERN_REF_TYPE;

                index = DecodeLeb128UInt32(index, &expr_len);
                if (!index){
                    //invalid LEB128 encoded
                    WpResultAddError(&result, WP_DIAG_ID_DECODE_LEB128_FAIL, W_DIAG_MOD_LIST_DECODER);
                    #if WASPC_CONFIG_DEV_FLAG == 1
                    //TODO
                    #endif
                    return result;                                       
                }
                elems[i].table_idx = expr_len;

                start_pos = index;
                index = DecodeExpr(index, sec->size);
                if (!index){
                    //invalid LEB128 encoded
                    WpResultAddError(&result, WP_DIAG_ID_DECODE_EXPR_FAIL, W_DIAG_MOD_LIST_DECODER);
                    #if WASPC_CONFIG_DEV_FLAG == 1
                    //TODO
                    #endif
                    return result;                                       
                }
                //TODO calc expr len
                elems[i].offset_len = expr_len;
                elems[i].offset = start_pos;

                elems[i].type = READ_BYTE();
                //TODO check elemkind is 0x00

                index = DecodeLeb128UInt32(index, &expr_len);
                if (!index){
                    //invalid LEB128 encoded
                    WpResultAddError(&result, WP_DIAG_ID_DECODE_LEB128_FAIL, W_DIAG_MOD_LIST_DECODER);
                    #if WASPC_CONFIG_DEV_FLAG == 1
                    //TODO
                    #endif
                    return result;                                       
                }
                elems[i].init_len = expr_len;
                elems[i].init = index;  
                break;
            // 7:u32 et : reftype el*:vec(expr) ⇒ {type 𝑒𝑡, init el*, mode declarative}
            case 7:

                elems[i].mode = WP_WAS_STRUC_MOD_ELEMENT_MODE_DECLARATIVE;
                elems[i].type = WAS_EXTERN_REF_TYPE;
                

                elems[i].offset_len = 0;
                elems[i].offset = NULL;

                index = DecodeLeb128UInt32(index, &expr_len);
                if (!index){
                    //invalid LEB128 encoded
                    WpResultAddError(&result, WP_DIAG_ID_DECODE_LEB128_FAIL, W_DIAG_MOD_LIST_DECODER);
                    #if WASPC_CONFIG_DEV_FLAG == 1
                    //TODO
                    #endif
                    return result;                                       
                }
                elems[i].init_len = expr_len;
                elems[i].init = index;

                break;
                
            default:
                WpResultAddError(&result, WP_DIAG_ID_INVALID_ELEMENT_TYPE, W_DIAG_MOD_LIST_DECODER);
                #if WASPC_CONFIG_DEV_FLAG == 1
                //TODO
                #endif
                return result;
            
        }       
        
    }
    
    if(index != buf_end){
        //Decode fails
        WpResultAddError(&result, WP_DIAG_ID_ERROR_DECODE_ELEM_SECTION, W_DIAG_MOD_LIST_DECODER);
        #if WASPC_CONFIG_DEV_FLAG == 1
        //TODO
        #endif
        return result;
    }

    mod->elems = elems;
    mod->elem_len = element_count;
    result.value.u32 = element_count;
    return result; 
    
    #undef READ_BYTE
    #undef NOT_END
}

/**
 * @brief function to decode the data count section on a binary webassembly module.
 * 
 * @param sec Binary section.
 * @param mod pointer to decoded mod instance.
 * @return WpResult error or u32 value if success.
 */
WpResult DecodeDataCountSection(const WasmBinSection *const sec, WpDecodedModule *mod){

    WpResult result;
    WpResultInit(&result);
    uint32_t data_count;
    const uint8_t *index = sec->content;                       // pointer to byte to traverse the binary file
    const uint8_t *buf_end = sec->content + sec->size;         // pointer to end of binary module
    
    //get type count
    index = DecodeLeb128UInt32(index, &data_count);
    if (!index){
        //invalid LEB128 encoded
        WpResultAddError(&result, WP_DIAG_ID_DECODE_LEB128_FAIL, W_DIAG_MOD_LIST_DECODER);
        #if WASPC_CONFIG_DEV_FLAG == 1
        //TODO
        #endif
        return result;                                       
    }   
    
    if(index != buf_end){
        //Decode fails
        WpResultAddError(&result, WP_DIAG_ID_ERROR_DECODE_DATA_COUNT_SECTION, W_DIAG_MOD_LIST_DECODER);
        #if WASPC_CONFIG_DEV_FLAG == 1
        //TODO
        #endif
        return result;
    }

    mod->data_count = data_count;
    result.value.u32 = data_count;
    return result;     
    
}

/**
 * @brief function to decode the code section on a binary webassembly module.
 * 
 * @param sec Binary section.
 * @param mod pointer to decoded mod instance. 
 * @return WpResult error or count of code entry decoded if succsess in u32.
 */
WpResult DecodeCodeSection(const WasmBinSection *const sec, WpDecodedModule *mod){

    WpResult result;
    WpResultInit(&result);
    Func *funcs = mod->funcs;
    const uint32_t func_len = mod->func_len;
    const uint8_t *index = sec->content;                                // pointer to byte to traverse the binary file
    const uint8_t *buf_end = sec->content + sec->size;                  // pointer to end of binary module    
    uint32_t code_count; 
    ;
    //ensure thar funcs is not null
    if(!funcs){
        //invalid LEB128 encoded
        
        WpResultAddError(&result, WP_DIAG_ID_ASSERT_DECODE_CODE_SECTION, W_DIAG_MOD_LIST_DECODER);
        #if WASPC_CONFIG_DEV_FLAG == 1
        //TODO
        #endif
        return result; 
    }
    
    //get code count
    index = DecodeLeb128UInt32(index, &code_count);
    if (!index){
        //invalid LEB128 encoded
        WpResultAddError(&result, WP_DIAG_ID_DECODE_LEB128_FAIL, W_DIAG_MOD_LIST_DECODER);
        #if WASPC_CONFIG_DEV_FLAG == 1
        //TODO
        #endif
        return result;                                       
    }
    
    if(code_count != func_len){
        //Mistmatch
        WpResultAddError(&result, WP_DIAG_ID_ASSERT_DECODE_CODE_SECTION, W_DIAG_MOD_LIST_DECODER);
        #if WASPC_CONFIG_DEV_FLAG == 1
        //TODO
        #endif
        return result; 
    }    
    
    for(uint32_t i = 0; i < code_count; i++){        
        
        index = DecodeCode(index, &funcs[i]);        
        if (!index){
            //invalid LEB128 encoded
            WpResultAddError(&result, WP_DIAG_ID_DECODE_CODE_FAIL, W_DIAG_MOD_LIST_DECODER);
            #if WASPC_CONFIG_DEV_FLAG == 1
            //TODO
            #endif
            return result;                                       
        }

    }
    
    if(index != buf_end){        
        //Decode fails
        WpResultAddError(&result, WP_DIAG_ID_ERROR_DECODE_CODE_SECTION, W_DIAG_MOD_LIST_DECODER);
        #if WASPC_CONFIG_DEV_FLAG == 1
        //TODO
        #endif
        return result;
    }

    result.value.u32 = code_count;
    return result; 
       
    
}

/**
 * @brief function to decode the data section on a binary webassembly module.
 * 
 * @param sec Binary section.
 * @param mod pointer to decoded mod instance.
 * @return WpResult error or edata array's length if succsess in u32.
 */
WpResult DecodeDataSection(const WasmBinSection * const sec, WpDecodedModule *mod){

    WpResult result;
    WpResultInit(&result);
    Data *datas;
    const uint8_t *index = sec->content;                       // pointer to byte to traverse the binary file
    const uint8_t *buf_end = sec->content + sec->size;                   // pointer to end of binary module
    const uint8_t *start_pos;
    uint32_t data_count;                                         // auxiliary variable    
    
    uint8_t byte_val;
    uint32_t mode_type;
    uint32_t expr_len;
    uint32_t i;

    //get type count
    index = DecodeLeb128UInt32(index, &data_count);
    if (!index){
        //invalid LEB128 encoded
        WpResultAddError(&result, WP_DIAG_ID_DECODE_LEB128_FAIL, W_DIAG_MOD_LIST_DECODER);
        #if WASPC_CONFIG_DEV_FLAG == 1
        //TODO
        #endif
        return result;                                       
    }
   
    //alocating elements on heap
    datas = ALLOCATE(Data, data_count);
    if (!datas){        
        //allocation check
        WpResultAddError(&result, WP_DIAG_ID_ALLOCATION_FAILURE, W_DIAG_MOD_LIST_DECODER);
        #if WASPC_CONFIG_DEV_FLAG == 1
        //TODO
        #endif
        return result;                                       
    }
    
    
    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    for(i = 0; i < data_count; i++){

        //get mode        
        index = DecodeLeb128UInt32(index, &mode_type);
        if (!index){
            //invalid LEB128 encoded
            WpResultAddError(&result, WP_DIAG_ID_DECODE_LEB128_FAIL, W_DIAG_MOD_LIST_DECODER);
            #if WASPC_CONFIG_DEV_FLAG == 1
            //TODO
            #endif
            return result;                                       
        }
        
        switch(mode_type) {
            /// 0:u32 𝑒:expr 𝑏*:vec(byte) ⇒ {init 𝑏*, mode active {memory 0, offset 𝑒}}
            case 0:
                
                datas[i].mode = WP_WAS_STRUC_MOD_DATA_MODE_ACTIVE;
                datas[i].mem_idx = 0;

                start_pos = index;
                index = DecodeExpr(index, sec->size);
                if (!index){
                    //invalid LEB128 encoded
                    WpResultAddError(&result, WP_DIAG_ID_DECODE_EXPR_FAIL, W_DIAG_MOD_LIST_DECODER);
                    #if WASPC_CONFIG_DEV_FLAG == 1
                    //TODO
                    #endif
                    return result;                                       
                }
                //TODO calc expr len
                datas[i].offset_len = expr_len;
                datas[i].offset = start_pos;

                index = DecodeLeb128UInt32(index, &expr_len);
                if (!index){
                    //invalid LEB128 encoded
                    WpResultAddError(&result, WP_DIAG_ID_DECODE_LEB128_FAIL, W_DIAG_MOD_LIST_DECODER);
                    #if WASPC_CONFIG_DEV_FLAG == 1
                    //TODO
                    #endif
                    return result;                                       
                }
                datas[i].init_len = expr_len;
                datas[i].init = index;

                break;
            /// 1:u32 𝑏*:vec(byte) ⇒ {init 𝑏*, mode passive}
            case 1:
                datas[i].mode = WP_WAS_STRUC_MOD_DATA_MODE_PASSIVE;
                datas[i].mem_idx = 0;

                datas[i].offset_len = 0;
                datas[i].offset = NULL;

                index = DecodeLeb128UInt32(index, &expr_len);
                if (!index){
                    //invalid LEB128 encoded
                    WpResultAddError(&result, WP_DIAG_ID_DECODE_LEB128_FAIL, W_DIAG_MOD_LIST_DECODER);
                    #if WASPC_CONFIG_DEV_FLAG == 1
                    //TODO
                    #endif
                    return result;                                       
                }
                datas[i].init_len = expr_len;
                datas[i].init = index;

                break;
            /// 2:u32 𝑥:memidx 𝑒:expr 𝑏*:vec(byte) ⇒ {init 𝑏*, mode active {memory 𝑥, offset 𝑒}}
            case 2:

                datas[i].mode = WP_WAS_STRUC_MOD_DATA_MODE_ACTIVE;
                //get memidx
                index = DecodeLeb128UInt32(index, &expr_len);
                if (!index){
                    //invalid LEB128 encoded
                    WpResultAddError(&result, WP_DIAG_ID_DECODE_LEB128_FAIL, W_DIAG_MOD_LIST_DECODER);
                    #if WASPC_CONFIG_DEV_FLAG == 1
                    //TODO
                    #endif
                    return result;                                       
                }
                datas[i].mem_idx = expr_len;

                start_pos = index;
                index = DecodeExpr(index, sec->size);
                if (!index){
                    //invalid LEB128 encoded
                    WpResultAddError(&result, WP_DIAG_ID_DECODE_EXPR_FAIL, W_DIAG_MOD_LIST_DECODER);
                    #if WASPC_CONFIG_DEV_FLAG == 1
                    //TODO
                    #endif
                    return result;                                       
                }
                datas[i].offset_len = expr_len;
                datas[i].offset = start_pos;

                index = DecodeLeb128UInt32(index, &expr_len);
                if (!index){
                    //invalid LEB128 encoded
                    WpResultAddError(&result, WP_DIAG_ID_DECODE_LEB128_FAIL, W_DIAG_MOD_LIST_DECODER);
                    #if WASPC_CONFIG_DEV_FLAG == 1
                    //TODO
                    #endif
                    return result;                                       
                }
                datas[i].init_len = expr_len;
                datas[i].init = index;

                break;           
            default:                
                //invalid Data mode
                WpResultAddError(&result, WP_DIAG_ID_INVALID_DATA_MODE, W_DIAG_MOD_LIST_DECODER);
                #if WASPC_CONFIG_DEV_FLAG == 1
                //TODO
                #endif
                return result;   
        }  
    }
    
    if(index != buf_end){
        //Decode fails
        WpResultAddError(&result, WP_DIAG_ID_ERROR_DECODE_DATA_SECTION, W_DIAG_MOD_LIST_DECODER);
        #if WASPC_CONFIG_DEV_FLAG == 1
        //TODO
        #endif
        return result;
    }

    mod->datas = datas;
    result.value.u32 = data_count;
    return result; 
    
    #undef READ_BYTE
    #undef NOT_END
}

//TODO conditional compilation for tis function
/**
 * @brief function for decode complete WasmBinModule object into a WasmModule.
 * 
 * @param bin_mod pointer to source wasm binary module
 * @param mod pointer to target wasm module
 * @return WpResult error or 1 in u32
 */
WpResult DecodeWpBinModule(const WpBinModule *const bin_mod, WpDecodedModule *mod){

    WpResult result;
    WpResultInit(&result);
    
    // If type sec is present:
    if(bin_mod->typesec.size > 0){
        result = DecodeTypeSection(&bin_mod->typesec, mod);      
        if(result.result_type == WP_OBJECT_RESULT_TYPE_ERROR){
            return result;
        }      
    }
    
    // If import sec is present:
    if(bin_mod->importsec.size > 0){
        result = DecodeImportSection(&bin_mod->importsec, mod);
        if(result.result_type == WP_OBJECT_RESULT_TYPE_ERROR){
            return result;
        } 
    }

    // If function sec is present:
    if(bin_mod->functionsec.size > 0){
        result = DecodeFunctionSection(&bin_mod->functionsec, mod);
        
        if(result.result_type == WP_OBJECT_RESULT_TYPE_ERROR){
            return result;
        } 
    }

    // If table sec is present:
    if(bin_mod->tablesec.size > 0){
        result = DecodeTableSection(&bin_mod->tablesec, mod);
        if(result.result_type == WP_OBJECT_RESULT_TYPE_ERROR){
            return result;
        } 
    }

    // If mem sec is present:
    if(bin_mod->memsec.size > 0){
        result = DecodeMemSection(&bin_mod->memsec, mod);
        if(result.result_type == WP_OBJECT_RESULT_TYPE_ERROR){
            return result;
        } 
    }

    // If global sec is present:
    if(bin_mod->globalsec.size > 0){
        result = DecodeGlobalSection(&bin_mod->globalsec, mod);
        if(result.result_type == WP_OBJECT_RESULT_TYPE_ERROR){
            return result;
        } 
    }
    
    // If export sec is present:
    if(bin_mod->exportsec.size > 0){
        result = DecodeExportSection(&bin_mod->exportsec, mod);
        if(result.result_type == WP_OBJECT_RESULT_TYPE_ERROR){
            return result;
        } 
           
    }

    // If start sec is present:
    if(bin_mod->startsec.size > 0){
        result = DecodeStartSection(&bin_mod->startsec, mod);
        if(result.result_type == WP_OBJECT_RESULT_TYPE_ERROR){
            return result;
        }         
    }

    // If element sec is present:
    if(bin_mod->elemsec.size > 0){
        result = DecodeElementSection(&bin_mod->elemsec, mod);
        if(result.result_type == WP_OBJECT_RESULT_TYPE_ERROR){
            return result;
        } 
    }

    // If data count sec is not empty
    if(bin_mod->datacountsec.size > 0){
        result = DecodeDataCountSection(&bin_mod->datasec, mod);
        if(result.result_type == WP_OBJECT_RESULT_TYPE_ERROR){
            return result;
        } 
    }
    
    // If code sec is present:
    if(bin_mod->codesec.size > 0){
       
        if(mod->func_len > 0){
            result = DecodeCodeSection(&bin_mod->codesec, mod);
            
            if(result.result_type == WP_OBJECT_RESULT_TYPE_ERROR){
                return result;
            } 
            if(result.value.u32 != mod->func_len){
                WpResultAddError(&result, WP_DIAG_ID_ASSERT_DECODE_CODE_SECTION, W_DIAG_MOD_LIST_DECODER);
                #if WASPC_CONFIG_DEV_FLAG == 1
                //TODO
                #endif
                return result; 
            }
        }
        else{
            WpResultAddError(&result, WP_DIAG_ID_ASSERT_DECODE_CODE_SECTION, W_DIAG_MOD_LIST_DECODER);
            #if WASPC_CONFIG_DEV_FLAG == 1
            //TODO
            #endif
            return result; 
        }
    }
    
    // If data sec is not empty
    if(bin_mod->datasec.size > 0){
        result = DecodeDataSection(&bin_mod->datasec, mod);
        if(result.result_type == WP_OBJECT_RESULT_TYPE_ERROR){
            return result;
        } 

        if(result.value.u32 != mod->data_count){
            WpResultAddError(&result, WP_DIAG_ID_ASSERT_DECODE_DATA_SECTION, W_DIAG_MOD_LIST_DECODER);
            #if WASPC_CONFIG_DEV_FLAG == 1
            //TODO
            #endif
            return result;
        }
    }

    result.value.u32 = 1;
    return result;

}