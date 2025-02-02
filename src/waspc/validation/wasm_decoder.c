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


#include "validation/wasm_decoder.h"
#include "validation/wasm_validator.h"
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
    uint32_t dec_u32;                                                        
    uint8_t byte_val;
    
    #define READ_BYTE() (*index++)

    //get index flag for maximun
    byte_val = READ_BYTE();
    index = DecodeLeb128UInt32(index, &dec_u32);           //get minimun value
    if (!index){
        lim->min = 0;
        lim->max = 0;
        return NULL;                                        
    }
    if(byte_val == 0){
        //maximun not present   
        lim->min = dec_u32;
        lim->max = 0;        
        return index;
    }
    else if (byte_val == 1){
        //maximun present        
        lim->min = dec_u32;
        
        index = DecodeLeb128UInt32(index, &dec_u32);           //get maximun value
        if (!index){
            lim->max= 0;
            return NULL;                                        
        }
        
        lim->max = dec_u32;        
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
 *
static const uint8_t * DecodeCodeLocals(const uint8_t *const code, const uint8_t *buf_end, Func *func){
    
    ///locals ::= 𝑛:u32 𝑡:valtype
    typedef struct _Locals {
        uint32_t n;
        uint8_t val_type;
    }_Locals;
    

    const uint8_t *index = code;    
    uint32_t local_count;
    uint32_t local_total = 0;
    uint32_t i = 0;
    uint32_t ii = 0;
    uint32_t iii = 0;
    _Locals *vector_local;

    //get local count 
    index = DecodeLeb128UInt32(index, &local_count);
    if (!index){
        return NULL;                                        
    }

    //Allocate memory to store temporary locals array
    vector_local = ALLOCATE(_Locals, local_count);
    if (!vector_local){        
        return NULL;                                       
    }

    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    for(i = 0; i < local_count; i++){        
        
        //get number of element n
        index = DecodeLeb128UInt32(index, &vector_local[i].n);
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

            local_total = local_total + vector_local[i].n;       //calc totals of local for field local_types_len
        }
        else{
            return NULL;
        }
        
    }

    //allocating space for function locals
    func->locals = ALLOCATE(ExtLocal, local_total);
    if(!func->locals){
        return NULL;
    }
    func->local_len = local_total;

    

    //traversing the local_vector array to fill locals array
    for(i = 0; i < local_count; i++){        
        for(ii = 0; ii < vector_local[i].n; ii++){
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
}/*

/**
 * @brief function to decode a code entry inside code section
 * 
 * @param code A binary segment for a encoded code non terminal symbol.
 *              code ::= size:u32 code:func  
 * @param func A function as Structure webassembly spec. See webassembly/structure/module.h
 * @return const uint8_t* pointer to next byte after code entry if success, otherwise NULL.
 *
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
    
}*/

/**
 * @brief Decode Type Section
 * 
 * @param mod pointer to Module State.
 * @return Pointer to module's type if success otherwise NULL.
 */
VecFuncType * DecodeTypeSection(WpModuleState *mod){

    WasmBinSection *sec = &mod->typesec;
    VecFuncType *types = &mod->was.types;                     // pounter to inner types section
    const uint8_t *index = sec->content;                        // pointer to byte to traverse the binary file
    const uint8_t *buf_end = sec->content + sec->size;          // pointer to end of binary module
    uint32_t length;                                            // auxiliary variable
    uint32_t param_count;
    uint32_t dec_u32;                                           // auxiliary variable to decode leb128 values
    uint8_t byte_value;                                         // auxiliary
    ValType encoded_type;

    
    //get vector length
    index = DecodeLeb128UInt32(index, &length);
    if (!index){        
        return NULL;                                       
    }  
    
    //alocating Funtypes on heap
    types->elements = ALLOCATE(FuncType, length);    
    if (!types->elements){             
        return NULL;                                        
    }  
    types->lenght = length;
    //TODO maybe is good idea make something similar to string internals (Crafting interpreter's book)
    // for function signature.
    
    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    for(uint32_t i = 0; i < length; i++){

        //get functype (0x60)
        encoded_type = READ_BYTE();
        if (encoded_type != WAS_FUNCT_TYPE){
            return NULL;                                    
        }
        
        //get parameters count        
        index = DecodeLeb128UInt32(index, &dec_u32);
        if (!index){
            return NULL;                                      
        }
        
        param_count = dec_u32;
        types->elements[i].params.lenght = param_count;

        //allocating array for parameters vector        
        types->elements[i].params.val_types = ALLOCATE(ValType, param_count);          
        if (!types->elements[i].params.val_types){        
            NULL;                                        
        } 

        //Loop for read parameter vector
        for(uint32_t ii = 0; ii < param_count; ii++){

            //get encoded parameter type            
            encoded_type = READ_BYTE();

            //check for valid type
            if(!ValidateValType(encoded_type)){
                return NULL;                                    
            }
            types->elements[i].params.val_types[ii] = encoded_type;
        }
        
        //getting result count
        index = DecodeLeb128UInt32(index, &dec_u32);
        if (!index){
            return NULL;                                    
        }        
        types->elements[i].results.lenght = dec_u32;
        param_count = dec_u32;
        //allocating array for result count
        types->elements[i].results.val_types = ALLOCATE(ValType, param_count);
        if (!types->elements[i].results.val_types){        
                                                   
        }

        //Loop for read result vector
        for(uint32_t ii = 0; ii < param_count; ii++){

            //get encoded parameter type            
            encoded_type = READ_BYTE();            
            if(!ValidateValType(encoded_type)){
                return NULL;                                    
            }
            types->elements[i].results.val_types[ii] = encoded_type;
            
        }  
          
        
    }
    
    if(index != buf_end){
         return NULL;
    }
    
    return types;
    
    #undef READ_BYTE
    #undef NOT_END
}

/**
 * @brief function to decode the import section in a binary webassembly module
 *
 * @param mod pointer to Module State.
 * @return Pointer to module's import if success otherwise NULL.
 */
VecImport * DecodeImportSection(WpModuleState *mod){
    
    WasmBinSection *sec = &mod->typesec;
    VecImport *imports = &mod->was.imports;
    const uint8_t *index = sec->content;                                // pointer to traverse the binary file
    const uint8_t *buf_end = sec->content + sec->size;                  // pointer to end of binary module
    uint32_t imports_count;
    uint32_t dec_u32;                                                   // auxiliary variable to decode leb128 values
    uint8_t byte_val;

    
    //get inports count
    index = DecodeLeb128UInt32(index, &imports_count);
    if (!index){
       return NULL;                                       
    }
       
    //alocating imports on heap
    imports->elements = ALLOCATE(Import, imports_count);
    if (!imports->elements){        
        return NULL;                                       
    }
    imports->lenght = imports_count;
           
    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    for(uint32_t i = 0; i < imports_count; i++){
        //get module name's vector
        index = DecodeLeb128UInt32(index, &dec_u32);
        if (!index){
           return NULL;                                       
        }
        
        imports->elements[i].module.lenght = dec_u32;
        imports->elements[i].module.name = (const char *)index;

        //moving index to name vector
        index = index + dec_u32;

        //get name's vector
        index = DecodeLeb128UInt32(index, &dec_u32);
        if (!index){
            //invalid LEB128 encoded           
            return NULL;                                        
        }
        
        imports->elements[i].name.lenght = dec_u32;
        imports->elements[i].name.name = index;

        //moving index to import desc
        index = index + dec_u32;               

        byte_val = READ_BYTE();
        switch (byte_val){
            case 0:                
                imports->elements[i].type = WP_WAS_STRUC_MOD_IMPORT_DESC_TYPE_FUNC;
                //decode function index
                index = DecodeLeb128UInt32(index, &dec_u32);
                if (!index){
                    //invalid LEB128 encoded           
                    return NULL;                                        
                }
                imports->elements[i].desc.x = dec_u32;
                break;
            case 1:
                imports->elements[i].type = WP_WAS_STRUC_MOD_IMPORT_DESC_TYPE_TABLE;
                //decode table type
                byte_val = READ_BYTE();     //read reftype
                if(byte_val != 0x70 || byte_val != 0x6F){
                    //invalid reftype
                    return NULL; 
                }
                imports->elements[i].desc.tt.et = byte_val;
                index = DecodedLimitsType(index, &imports->elements[i].desc.tt.lim);
                if(!index){
                    //invalid limit type
                    return NULL; 
                }
                break;
            case 2:
                imports->elements[i].type = WP_WAS_STRUC_MOD_IMPORT_DESC_TYPE_MEM;
                index = DecodedLimitsType(index, &imports->elements[i].desc.mt);
                break;
            case 3:
                imports->elements[i].type = WP_WAS_STRUC_MOD_IMPORT_DESC_TYPE_GLOBAL;
                //decode global type
                byte_val = READ_BYTE();
                if(!ValidateValType(byte_val)){
                    return NULL;                                    
                }
                imports->elements[i].desc.gt.t = byte_val;
                byte_val = READ_BYTE();
                if(byte_val != 0 || byte_val != 1){
                    //invalid mutability
                    return NULL; 
                }
                imports->elements[i].desc.gt.m = byte_val;
                break;
            default:
                //invalid desc type                
                return NULL; 
        }            


    }
    
    if(index != buf_end){
        return NULL;
    }

    return imports;        
    #undef READ_BYTE
    #undef NOT_END
}

/**
 * @brief function to decode the function section in a binary webassembly module.
 * 
 * @param mod pointer to Module State.
 * @return Pointer to module's func if success otherwise NULL.
 */
VecFunc * DecodeFunctionSection(WpModuleState *mod){

    WasmBinSection *sec = &mod->functionsec;
    VecFunc *funcs = &mod->was.funcs;                                          // pounter to inner function section    
    const uint8_t *index = sec->content;                                        // pointer to byte to traverse the binary file
    const uint8_t *buf_end = sec->content + sec->size;                          // pointer to end of binary module
    uint32_t func_count;                                                        // auxiliary variable    
    uint32_t dec_u32;                                                           // auxiliary variable to decode leb128 values
    uint8_t byte_val;
    ValType encoded_type;
    

    //get functions count
    index = DecodeLeb128UInt32(index, &func_count);
    if (!index){
        return NULL;                                       
    }
   
    //alocating types on heap
    funcs->elements = ALLOCATE(Func, func_count);
    if (!funcs->elements){        
        return NULL;                                      
    }    
    funcs->lenght = func_count;
    
      
    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    for(uint32_t i = 0; i < func_count; i++){        
        
        //get index        
        index = DecodeLeb128UInt32(index, &dec_u32);
        if (!index){
            return NULL;                                     
        }
               
        //Init values
        if(dec_u32 >= mod->was.types.lenght){
            //invalid type index
            return NULL; 
        }
        funcs->elements[i].type = &mod->was.types.elements[dec_u32];        //getting address of type

    }
    
    if(index != buf_end){
        return NULL;
    }

    return funcs;
        
    #undef READ_BYTE
    #undef NOT_END
}

/**
 * @brief function to decode the table section in a binary webassembly module.
 * 
 * @param mod pointer to Module State.
 * @return Pointer to module's table if success otherwise NULL.
 */
VecTable * DecodeTableSection(WpModuleState *mod){

    
    WasmBinSection *sec = &mod->tablesec;
    VecTable *tables = &mod->was.tables;
    const uint8_t *index = sec->content;                                        // pointer to byte to traverse the binary file
    const uint8_t *buf_end = sec->content + sec->size;                          // pointer to end of binary module
    uint32_t table_count;                                                       // auxiliary variable
    
    uint32_t dec_u32;                                                           // auxiliary variable to decode leb128 values
    uint8_t byte_val;
    Limits lim;

    //get table count
    index = DecodeLeb128UInt32(index, &table_count);
    if (!index){        
        return NULL;                                       
    }
       
    //alocating types on heap
    tables->elements = ALLOCATE(TableType, table_count);
    if (!tables->elements){        
        return NULL;                                       
    }   
    tables->lenght = table_count;

    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    for(uint32_t i = 0; i < table_count; i++){        
        
        //get reference type       0x6F->external, 0x70->function ref
        byte_val = READ_BYTE();

        if(byte_val == 0x6F){
            //external reference
            tables->elements[i].et = 0x6F;            
        }
        else if(byte_val == 0x70){
            tables->elements[i].et = 0x7F; 
        }
        else{
            return NULL; 
        }
        
        index = DecodedLimitsType(index, &tables->elements[i].lim);
        if(index == NULL){   
            return NULL;  
        }   
    }
    
    if(index != buf_end){
        return NULL; 
    }
    
    return tables; 
    
    #undef READ_BYTE
    #undef NOT_END
}

/**
 * @brief function to decode the mem section in a binary webassembly module.
 * 
 * @param mod pointer to Module State.
 * @return Pointer to module's mem if success otherwise NULL.
 */
VecMem * DecodeMemSection(WpModuleState *mod){

    WasmBinSection *sec = &mod->memsec;
    VecMem *mems = &mod->was.mems;
    const uint8_t *index = sec->content;                                        // pointer to byte to traverse the binary file
    const uint8_t *buf_end = sec->content + sec->size;                      // pointer to end of binary module
    uint32_t mem_count;                                                            // auxiliary variable    
    uint32_t dec_u32;                                                       // auxiliary variable to decode leb128 values
    uint8_t byte_val;
    
       
    Limits lim;

    //get mem count
    index = DecodeLeb128UInt32(index, &mem_count);
    if (!index){
        return NULL;                                 
    }
   
    //alocating types on heap
    mems->elements = ALLOCATE(MemType, mem_count);
    if (!mems->elements){        
        return NULL;                                            
    }  
    mems->lenght = mem_count;

    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    for(uint32_t i = 0; i < mem_count; i++){       
        
        
        index = DecodedLimitsType(index, &mems->elements[i]);

        if(index == NULL){             
            return NULL;  
        }
        

    }
    
    if(index != buf_end){        
        return NULL; 
    }

    return mems; 
    
    #undef READ_BYTE
    #undef NOT_END
}

/**
 * @brief function to decode the global section in a binary webassembly module.
 * 
 * @param mod pointer to Module State.
 * @return Pointer to module's global if success otherwise NULL.
 */
VecGlobal * DecodeGlobalSection(WpModuleState *mod){
    
    WasmBinSection *sec = &mod->globalsec;
    VecGlobal *globals = &mod->was.globals;
    const uint8_t *index = sec->content;                                    // pointer to byte to traverse the binary file
    const uint8_t *buf_end = sec->content + sec->size;                      // pointer to end of binary module
    uint32_t global_count;                                            
    uint32_t dec_u32;                                                       // auxiliary variable
    uint8_t byte_val;
    ValType encoded_type;
        

    //get type count
    index = DecodeLeb128UInt32(index, &global_count);
    if (!index){
        //invalid LEB128 encoded
        return NULL;                                    
    }
   
    //alocating types on heap
    globals->elements = ALLOCATE(Global, global_count);
    if (!globals->elements){        
        return NULL;                                   
    } 
    globals->lenght = global_count;
    
    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    for(uint32_t i = 0; i < global_count; i++){

        //get type
        encoded_type = READ_BYTE();
        //check for valid type
        if(!ValidateValType(encoded_type)){
            return NULL;                                    
        }
        globals->elements[i].gt.t = encoded_type;
        
        //get mutable property
        byte_val = READ_BYTE();
        if(byte_val == 0 || byte_val == 1){
            globals->elements[i].gt.m = byte_val;
        }        
        else{
           return NULL;
        }
        

        globals->elements[i].e.instr = index;
        index = DecodeExpr(index, buf_end-index);        
        if (!index){            
            return NULL;                                    
        }        
        globals->elements[i].e.end = index;
    }
    
    if(index != buf_end){
        return NULL;
    }
    
    return globals; 
    
    #undef READ_BYTE
    #undef NOT_END
}

/**
 * @brief function to decode the export section on a binary webassembly module.
 * 
 * @param mod pointer to Module State.
 * @return Pointer to module's export if success otherwise NULL.
 */
VecExport * DecodeExportSection(WpModuleState *mod){

    WasmBinSection *sec = &mod->exportsec;
    VecExport *exports = &mod->was.exports;
    const uint8_t *index = sec->content;                                // pointer to byte to traverse the binary file
    const uint8_t *buf_end = sec->content + sec->size;                 // pointer to end of binary module
    uint32_t export_count;
    uint32_t dec_u32;                                                // auxiliary variable to decode leb128 values
    uint8_t byte_val;
    
    //get inports count
    index = DecodeLeb128UInt32(index, &export_count);
    if (!index){
        return NULL;                                     
    }
        
    //alocating imports on heap
    exports->elements = ALLOCATE(Export, export_count);
    if (!exports->elements){        
        return NULL;                                    
    } 
    exports->lenght = export_count;
    
    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)
    
    for(uint32_t i = 0; i < export_count; i++){
        
        //get name's vector
        index = DecodeLeb128UInt32(index, &dec_u32);
        if (!index){
            return NULL;                                      
        }  

        exports->elements[i].nm.lenght = dec_u32;        
        exports->elements[i].nm.name = index;
        //moving index to import desc
        index = index + dec_u32;

        //Get export description        
        byte_val = READ_BYTE();
        switch (byte_val){
            case 0:                
                exports->elements[i].d.type = WP_WAS_STRUC_MOD_EXPORT_DESC_TYPE_FUNC;
                break;
            case 1:
                exports->elements[i].d.type = WP_WAS_STRUC_MOD_EXPORT_DESC_TYPE_TABLE;
                break;
            case 2:
                exports->elements[i].d.type = WP_WAS_STRUC_MOD_EXPORT_DESC_TYPE_MEM;
                break;
            case 3:
                exports->elements[i].d.type = WP_WAS_STRUC_MOD_EXPORT_DESC_TYPE_GLOBAL;
                break;
            default:
                return NULL;
        }

        //get desc index
        index = DecodeLeb128UInt32(index, &dec_u32);
        if (!index){
            return NULL;                                      
        }
        exports->elements[i].d.x = dec_u32;
    }
    
    if(index != buf_end){
        //Decode fails       
        return NULL;
    }

    return exports; 
    
    #undef READ_BYTE
    #undef NOT_END
}

/**
 * @brief function to decode the start section on a binary webassembly module.
 * 
 * @param mod pointer to Module State.
 * @return Pointer to module's start if success otherwise NULL.
 */
uint32_t * DecodeStartSection(WpModuleState *mod){

    WasmBinSection *sec = &mod->startsec;
    uint32_t *start = &mod->was.start;
    const uint8_t *index = sec->content;                      // pointer to byte to traverse the binary file
    const uint8_t *buf_end = sec->content + sec->size;                 // pointer to end of binary module    
  
    
    //get start index
    index = DecodeLeb128UInt32(index, start);
    if (!index){
        return NULL;                                      
    }
    
    if(index != buf_end){
        return NULL;
    }
    
    return start;     
    
}

/**
 * @brief function to decode the element section on a binary webassembly module.
 * 
 * @param mod pointer to Module State.
 * @return Pointer to module's elem if success otherwise NULL.
 *
VecElem * DecodeElementSection(WpModuleState *mod){

    WasmBinSection *sec = &mod->elemsec;
    VecElem *elem = &mod->was.elem;
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
        return NULL;                                     
    }   
   
    //alocating elements on heap
    elem->elements = ALLOCATE(Elem, element_count);
    if (!elem->elements){        
        return NULL;                                       
    } 
    elem->lenght = element_count;
    
    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    for(uint32_t i = 0; i < element_count; i++){

        //get type        
        index = DecodeLeb128UInt32(index, &encoded_type);
        if (!index){
            return NULL;                                      
        }
        
        switch(encoded_type) {
            /// 0:u32 𝑒:expr 𝑦*:vec(funcidx) ⇒ {type funcref, init ((ref.func 𝑦) end)*, mode active {table 0, offset 𝑒}}
            case 0:
                //reference function instruction
                uint8_t expr_ref[6] = {0xD2, 0x00, 0x00, 0x00, 0x00, 0x0B};

                elem->elements[i].mode = WP_WAS_STRUC_MOD_ELEMENT_MODE_ACTIVE;
                elem->elements[i].type = WAS_REFTYPE_FUNCREF;
                elem->elements[i].active.table_idx = 0;

                //decoding e
                start_pos = index;
                index = DecodeExpr(index, sec->size);
                if (!index){                    
                    return NULL;                                       
                }
                elem->elements[i].active.offset.end = index;
                elem->elements[i].active.offset.instr = start_pos;

                //decoding vector y* 
                index = DecodeLeb128UInt32(index, &expr_len);
                if (!index){
                    return NULL;                                       
                }

                //alocating elements on heap
                elem->elements[i].init.elements = ALLOCATE(Expr, element_count);
                if (elem->elements[i].init.elements == NULL){        
                    return NULL;                                       
                } 
                elem->elements[i].init.lenght = expr_len;
                for (uint32_t ii = 0; ii < expr_len; ii++)
                {
                     
                }
                          
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
}*/

/**
 * @brief function to decode the data count section on a binary webassembly module.
 * 
 * @param sec Binary section.
 * @param mod pointer to decoded mod instance.
 * @return WpResult error or u32 value if success.
 *
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
    
}*/

/**
 * @brief function to decode the code section on a binary webassembly module.
 * 
 * @param sec Binary section.
 * @param mod pointer to decoded mod instance. 
 * @return WpResult error or count of code entry decoded if succsess in u32.
 *
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
       
    
}*/

/**
 * @brief function to decode the data section on a binary webassembly module.
 * 
 * @param sec Binary section.
 * @param mod pointer to decoded mod instance.
 * @return WpResult error or edata array's length if succsess in u32.
 *
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
}*/

//TODO conditional compilation for tis function
/**
 * @brief function for decode complete WasmBinModule object into a WasmModule.
 * 
 * @param bin_mod pointer to source wasm binary module
 * @param mod pointer to target wasm module
 * @return WpResult error or 1 in u32
 *
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

}*/