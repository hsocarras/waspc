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
#include "utils/leb128.h"
#include "webassembly/binary/instructions.h"

#include <assert.h>
//#include <stdio.h>

/**
 * @brief  Function to decode expr rule
 * expr ::= (in:instr)* 0x0B ⇒ in* end
 * 
 * @param buf segment for a binary web asembly module
 * @param max_len max len to traverse the buffer
 * @return const uint8_t* pointer to next byte after expr
 * @example
 *  uint8_t binary_module[] = {0x41, 0x01, 0x0B, 0x20, 0x00, 0x0B}; // Example binary module segment
    const uint8_t *result = SkipExprBuf(binary_module, sizeof(binary_module));
    if (result) {
        printf("Next byte after expr: 0x%02X\n", *result);
    } else {
        printf("Failed to decode expr or reached max length without finding END.\n");
    }
 */  
const uint8_t * SkipExprBuf(const uint8_t *const buf, const uint32_t max_len){

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
 *
static const uint8_t * SkipLimitTypeBuf(const uint8_t *const buf, Limits *lim){

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

}*/

const uint8_t * SkipLimitTypeBuf(const uint8_t *const buf){
    const uint8_t *index = buf;
    uint32_t aux;
    switch (*index)
    {
    case 0:
        index++;
        index = DecodeLeb128UInt32(index, &aux);
        if(!index){
            return NULL;
        }
        break;   
    case 1:
        index++;
        index = DecodeLeb128UInt32(index, &aux);
        if(!index){
            return NULL;
        }
        index = DecodeLeb128UInt32(index, &aux);
        if(!index){
            return NULL;
        }
    default:
        return NULL;
        break;
    }

    return index;
}
/**
 * @brief function to func rule on code section
 *              func ::= (𝑡*)*:vec(locals) 𝑒:expr ⇒ concat((𝑡*)*), 𝑒 (if |concat((𝑡*)*)| < 232)
 *              locals ::= 𝑛:u32 𝑡:valtype ⇒ 𝑡𝑛
 * @param code pointer to binary code segment
 * @param code_entry Pointer to a Code structure.
 * @return const uint8_t* pointer to next Code segment, otherwise NULL.
 *
static const uint8_t * DecodeCodeFunc(const uint8_t *code, Code *code_entry){
    
    
    CodeFunc *func = &code_entry->code;
    const uint8_t *index = code;    
    const uint8_t *buf_end = code + code_entry->size;
    uint32_t local_count;
    uint32_t dec_u32;
    uint8_t byte_val;
    uint32_t i = 0;
    

    //get local count 
    index = DecodeLeb128UInt32(index, &local_count);
    if (!index){
        return NULL;                                        
    }
    func->locals.lenght = local_count;    
    func->locals.elements = ALLOCATE(Locals, local_count);
    if (!func->locals.elements){        
        return NULL;                                      
    }
    
    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    //decoding locals
    for(i = 0; i < local_count; i++){        
        
        //get number of element n
        index = DecodeLeb128UInt32(index, &dec_u32);
        if (!index){        
            return NULL;                                     
        }
        func->locals.elements[i].n = dec_u32;
        
        //get type
        byte_val = READ_BYTE();  
        if(ValidateValType(byte_val)){
            func->locals.elements[i].t = byte_val;             
        }        
        else{            
            return NULL;
        }
        

    }

    //decode expression
    func->e.instr = index;
    index = SkipExprBuf(index, buf_end - index);
    if (!index){
        return NULL;                                      
    }
    
    func->e.end = index-1; //Last byte is 0x0B opcode
     //Last byte code must be END
    if(*(index-1) != OPCODE_END){
        //Decode fails instruction must be end with 0x0B        
        return NULL;        
    }
    //free vector_local;
    //FREE_MEM(vector_local);

    return index;

    #undef READ_BYTE
    #undef NOT_END
}*/

/**
 * @brief function to decode a code entry inside code section
 * 
 * @param code A binary segment for a encoded code non terminal symbol.
 *              code ::= size:u32 code:func  
 * @param code_entry Pointer to a Code structure.
 * @return const uint8_t* pointer to next Code segment, otherwise NULL.
 *
static const uint8_t * DecodeCode(const uint8_t * code, Code *code_entry){
        
    const uint8_t *index = code;           // pointer to end of binary module      
    uint32_t dec_u32;    
    uint32_t i;
    uint32_t ii;
    uint32_t iii=0;
    
    //get code segment size
    index = DecodeLeb128UInt32(index, &dec_u32);    
    if (!index){
        return NULL;                                        
    } 
    code_entry->size = dec_u32;
    
    //Decoding func
    index = DecodeCodeFunc(index, code_entry);
    if (!index){          
        return NULL;                                        
    }
        
    //pointter to next byte for next code segment    
    return index;   
    
}*/

/**
 * @brief Decode Type Section
 * 
 * @param mod pointer to Module State.
 * @return Pointer to module's type if success otherwise NULL.
 *
VecFuncType * DecodeTypeSection(WpModuleState *mod){

    WasmBinSection *sec = &mod->typesec;
    VecFuncType *types = &mod->was->types;                     // pounter to inner types section
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
}*/

/**
 * @brief function to get a function type by index
 * @param typesec. Pointer to the WasmBinSection containing the type section.
 * @param type_index index of the function type to get
 * @return uint8_t* pointer to the function type (0x60) or NULL if not found
 * 
 * @example
 *  WasmBinSection type_section = { .size = ..., .content = ... };
    uint32_t func_type_index = 2; // Example index
    const uint8_t *func_type_ptr = GetFuncTypeByIndex(type_section, func_type_index);
    if (func_type_ptr) {
        printf("Function type found at address: %p\n", func_type_ptr);
    } else {
        printf("Function type not found for index %u\n", func_type_index);
    }
 */
const uint8_t * GetFuncTypeByIndex(WasmBinSection typesec, uint32_t type_index){    
     
    assert(typesec.content != NULL);                                    // Check if typesec content is not NULL

    const uint8_t *index = typesec.content;                        // pointer to byte to traverse the binary file
    const uint8_t *buf_end = index + typesec.size;                      // pointer to end of section
    uint32_t length;                                                    // auxiliary variable    
    uint32_t dec_u32;                                                   // auxiliary variable to decode leb128 values
    ValType encoded_type;

    
    //get vector length
    index = DecodeLeb128UInt32(index, &length);
    if (!index){        
        return NULL;                                       
    }  
    
   // Check if type_index is valid
    if (type_index >= length) {
        return NULL; // Invalid type index
    }
    
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

        if(i == type_index){
            return index-1; // Return pointer to the start of the function type 0x60
        }        
        
        //get parameters count        
        index = DecodeLeb128UInt32(index, &dec_u32);
        if (!index){
            return NULL;                                      
        }
        
        index += dec_u32; // Skip over the parameter types 
                
        //getting result count
        index = DecodeLeb128UInt32(index, &dec_u32);
        if (!index){
            return NULL;                                    
        }        
        index += dec_u32; // Skip over the result types       

        if(index > buf_end){
            return NULL; // If index exceeds buffer end, return NULL
        }
        
    }
    
        
    return NULL; // If we reach here, type_index was not found
    
    #undef READ_BYTE
    #undef NOT_END
}

/**
 * @brief function to decode the import section in a binary webassembly module
 *
 * @param mod pointer to Module State.
 * @return Pointer to module's import if success otherwise NULL.
 *
VecImport * DecodeImportSection(WpModuleState *mod){
    
    WasmBinSection *sec = &mod->typesec;
    VecImport *imports = &mod->was->imports;
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
        imports->elements[i].name.name = (const char *)index;

        //moving index to import desc
        index = index + dec_u32;               

        byte_val = READ_BYTE();
        switch (byte_val){
            case 0:                
                imports->elements[i].type = WP_EXTERNAL_TYPE_FUNC;
                //decode function index
                index = DecodeLeb128UInt32(index, &dec_u32);
                if (!index){
                    //invalid LEB128 encoded           
                    return NULL;                                        
                }
                imports->elements[i].desc.x = dec_u32;
                break;
            case 1:
                imports->elements[i].type = WP_EXTERNAL_TYPE_TABLE;
                //decode table type
                byte_val = READ_BYTE();     //read reftype
                if(byte_val != 0x70 || byte_val != 0x6F){
                    //invalid reftype
                    return NULL; 
                }
                imports->elements[i].desc.tt.et = byte_val;
                index = SkipLimitTypeBuf(index, &imports->elements[i].desc.tt.lim);
                if(!index){
                    //invalid limit type
                    return NULL; 
                }
                break;
            case 2:
                imports->elements[i].type = WP_EXTERNAL_TYPE_MEMORY;
                index = SkipLimitTypeBuf(index, &imports->elements[i].desc.mt);
                break;
            case 3:
                imports->elements[i].type = WP_EXTERNAL_TYPE_GLOBAL;
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
}*/

/**
 * @brief function to get an import by index
 * @param importsec. Pointer to the WasmBinSection containing the import section.
 * @param import_index index of the import to get
 * @return uint8_t* pointer to the import or NULL if not found
 */
const uint8_t * GetImportByIndex(WasmBinSection importsec, uint32_t import_index){

    assert(importsec.content != NULL);                                    // Check if importsec content is not NULL

    const uint8_t *index = importsec.content;                        // pointer to byte to traverse the binary file
    const uint8_t *buf_end = index + importsec.size;                      // pointer to end of section
    uint32_t length;                                                    // auxiliary variable    
    uint32_t dec_u32;                                                   // auxiliary variable to decode leb128 values
    uint8_t encoded_type;

    
    //get vector length
    index = DecodeLeb128UInt32(index, &length);
    if (!index){        
        return NULL;                                       
    }  
    
   // Check if import_index is valid
    if (import_index >= length) {
        return NULL; // Invalid import index
    }
    
    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    for(uint32_t i = 0; i < length; i++){

        
        if(i == import_index){
            return index; // Return pointer to the start of the import type 0x02
        }        
        
        //get module name's vector
        index = DecodeLeb128UInt32(index, &dec_u32);
        if (!index){
            return NULL;                                      
        }        
        index += dec_u32; // Skip over the module name vector
        
        //get name's vector
        index = DecodeLeb128UInt32(index, &dec_u32);
        if (!index){
            return NULL;                                    
        }        
        index += dec_u32; // Skip over the name vector

        encoded_type = READ_BYTE(); // Read the type byte
        switch (encoded_type) {
            case 0: 
                index = DecodeLeb128UInt32(index, &dec_u32); // Function index
                if (!index) {
                    return NULL;
                }
                index += dec_u32; // Skip over the function index
                break;
            case 1:
                index++; // Skip reftype byte
                index = SkipLimitTypeBuf(index); // Table limits
                if (!index) {
                    return NULL;
                }
                break;
            case 2:
                index = SkipLimitTypeBuf(index); // Memory limits
                if (!index) {
                    return NULL;
                }
                break;
            case 3:
                encoded_type = READ_BYTE(); // Global type
                
                index++; // Skip mutability byte
                break;
            default:
                return NULL; // Invalid import type}
        }

        if(index > buf_end){
            return NULL; // If index exceeds buffer end, return NULL
        }        
    }
    return NULL;  //never reach this line
}


/**
 * @brief function to decode the function section in a binary webassembly module.
 * 
 * @param mod pointer to Module State.
 * @return Pointer to module's func if success otherwise NULL.
 *
VecFunc * DecodeFunctionSection(WpModuleState *mod){

    WasmBinSection *sec = &mod->functionsec;
    VecFunc *funcs = &mod->was->funcs;                                          // pounter to inner function section    
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
        if(dec_u32 >= mod->was->types.lenght){
            //invalid type index 
            return NULL; 
        }
        funcs->elements[i].type_index = dec_u32;        //getting address of type
        funcs->elements[i].locals.lenght = 0;           //init locals lenght
        funcs->elements[i].locals.elements = NULL;       //init locals vector
        funcs->elements[i].body.instr = NULL;           //init body instr
        funcs->elements[i].body.end = NULL;             //init body end

    }
    
    if(index != buf_end){
        return NULL;
    }

    return funcs;
        
    #undef READ_BYTE
    #undef NOT_END
}*/


/**
 * @brief function to get a function by index
 * @param functionsec. Pointer to the WasmBinSection containing the function section.
 * @param function_index index of the function to get
 * @return uint8_t* pointer to the function type index or NULL if not found
 */ 
const uint8_t * GetFunctionByIndex(WasmBinSection functionsec, uint32_t function_index){
    
    assert(functionsec.content != NULL);

    const uint8_t *index = functionsec.content;                                        // pointer to byte to traverse the binary file
    const uint8_t *buf_end = index + functionsec.size;                          // pointer to end of binary module
    uint32_t length;                                                        // auxiliary variable    
    uint32_t dec_u32;                                                           // auxiliary variable to decode leb128 values
    uint8_t byte_val;
    ValType encoded_type;
    

    //get functions count
    index = DecodeLeb128UInt32(index, &length);
    if (!index){
        return NULL;                                       
    }   
    // Check if import_index is valid
    if (function_index >= length) {
        return NULL; // Invalid function index
    }
    
      
    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    for(uint32_t i = 0; i < length; i++){        
        
        if(function_index == i){
            return index;
        }
        //get index        
        index = DecodeLeb128UInt32(index, &dec_u32);
        if (!index){
            return NULL;                                     
        }
        if(index > buf_end){
            return NULL; // If index exceeds buffer end, return NULL
        }
    }
    
    return NULL;    //never reach this line

    #undef READ_BYTE
    #undef NOT_END

}


/**
 * @brief function to decode the table section in a binary webassembly module.
 * 
 * @param mod pointer to Module State.
 * @return Pointer to module's table if success otherwise NULL.
 *
VecTable * DecodeTableSection(WpModuleState *mod){

    
    WasmBinSection *sec = &mod->tablesec;
    VecTable *tables = &mod->was->tables;
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
        
        index = SkipLimitTypeBuf(index, &tables->elements[i].lim);
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
}*/


/**
 * @brief function to get a table by index
 * @param tablesec. Pointer to the WasmBinSection containing the table section.
 * @param table_index index of the table to get
 * @return uint8_t* pointer to the table type (0x70 or 0x6F) or NULL if not found
 */ 
const uint8_t * GetTableByIndex(WasmBinSection tablesec, uint32_t table_index){

    assert(tablesec.content != NULL);                                    // Check if tablesec content is not NULL
    const uint8_t *index = tablesec.content;                                        // pointer to byte to traverse the binary file
    const uint8_t *buf_end = index + tablesec.size;                          // pointer to end of binary module
    uint32_t length;                                                       // auxiliary variable    
    uint32_t dec_u32;                                                           // auxiliary variable to decode leb128 values
    uint8_t byte_val;
    Limits lim;

    //get table count
    index = DecodeLeb128UInt32(index, &length);
    if (!index){        
        return NULL;                                       
    }
    // Check if table_index is valid
    if (table_index >= length) {
        return NULL; // Invalid table index
    }      
    

    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    for(uint32_t i = 0; i < length; i++){        
        
        //get reference type       0x6F->external, 0x70->function ref
        byte_val = READ_BYTE();
        if(i == table_index){
            if(byte_val != 0x6F && byte_val != 0x70){
                return NULL; // Invalid reference type
            }
            return index-1; // Return pointer to the start of the table type 0x70 or 0x6F
        }        
        
        index = SkipLimitTypeBuf(index);
        if(index == NULL){   
            return NULL;  
        }   

        if(index >= buf_end){
            return NULL; 
        }
    }

    return NULL; // If we reach here, table_index was not found
    
    #undef READ_BYTE
    #undef NOT_END
}


/**
 * @brief function to decode the mem section in a binary webassembly module.
 * 
 * @param mod pointer to Module State.
 * @return Pointer to module's mem if success otherwise NULL.
 *
VecMem * DecodeMemSection(WpModuleState *mod){

    WasmBinSection *sec = &mod->memsec;
    VecMem *mems = &mod->was->mems;
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
        
        
        index = SkipLimitTypeBuf(index, &mems->elements[i]);

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
}*/

/**
 * @brief function to get a memory by index
 * @param memsec. Pointer to the WasmBinSection containing the memory section.
 * @param mem_index index of the memory to get
 * @return uint8_t* pointer to the memory type or NULL if not found
 */
const uint8_t * GetMemByIndex(WasmBinSection memsec, uint32_t mem_index){

    assert(memsec.content != NULL);                                         // Check if memsec content is not NULL
    const uint8_t *index = memsec.content;                                  // pointer to byte to traverse the binary file
    const uint8_t *buf_end = index + memsec.size;                           // pointer to end of binary module
    uint32_t length;                                                        // auxiliary variable    
    uint32_t dec_u32;                                                       // auxiliary variable to decode leb128 values
    uint8_t byte_val;
    
       
    Limits lim;

    //get mem count
    index = DecodeLeb128UInt32(index, &length);
    if (!index){        
        return NULL;                                       
    }
    // Check if mem_index is valid
    if (mem_index >= length) {
        return NULL; // Invalid memory index
    }      
    

    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    for(uint32_t i = 0; i < length; i++){        
        
        if(i == mem_index){
            return index; // Return pointer to the start of the memory type
        }

        index = SkipLimitTypeBuf(index);
        if(index == NULL){   
            return NULL;  
        }   

        if(index >= buf_end){
            return NULL; 
        }
        
    }

    return NULL; // If we reach here, mem_index was not found
    
    #undef READ_BYTE
    #undef NOT_END
}

/**
 * @brief function to decode the global section in a binary webassembly module.
 * 
 * @param mod pointer to Module State.
 * @return Pointer to module's global if success otherwise NULL.
 *
VecGlobal * DecodeGlobalSection(WpModuleState *mod){
    
    WasmBinSection *sec = &mod->globalsec;
    VecGlobal *globals = &mod->was->globals;
    const uint8_t *index = sec->content;                                    // pointer to byte to traverse the binary file
    const uint8_t *buf_end = sec->content + sec->size;                      // pointer to next section's first byte
    uint32_t global_count;                                                  // auxiliary variable for global count
    uint32_t dec_u32;                                                       // auxiliary variable for decode leb128 values
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
        index = SkipExprBuf(index, buf_end-index);        
        if (!index){            
            return NULL;                                    
        }        
        globals->elements[i].e.end = index - 1; // last byte must be OPCODE_END
    }
    
    if(index != buf_end){
        return NULL;
    }
    
    return globals; 
    
    #undef READ_BYTE
    #undef NOT_END
}*/

/**
 * @brief function to get a global by index
 * @param globalsec. Pointer to the WasmBinSection containing the global section.
 * @param global_index index of the global to get
 * @return uint8_t* pointer to the global type or NULL if not found
 */
const uint8_t * GetGlobalByIndex(WasmBinSection globalsec, uint32_t global_index){

    assert(globalsec.content != NULL);                                  // Check if globalsec content is not NULL

    const uint8_t *index = globalsec.content;                           // pointer to byte to traverse the binary file
    const uint8_t *buf_end = index + globalsec.size;                    // pointer to end of section
    uint32_t length;                                                    // auxiliary variable    
    uint32_t dec_u32;                                                   // auxiliary variable to decode leb128 values
    ValType encoded_type;

    
    //get vector length
    index = DecodeLeb128UInt32(index, &length);
    if (!index){        
        return NULL;                                       
    }  
    
   // Check if global_index is valid
    if (global_index >= length) {
        return NULL; // Invalid global index
    }
    
      
    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    for(uint32_t i = 0; i < length; i++){

        //get type        
        encoded_type = READ_BYTE();       

        if(i == global_index){
            return index-1; // Return pointer to the start of the global type
        }  
       
        index++; // Skip mutable byte
        
        index = SkipExprBuf(index, buf_end - index); // Skip over the expression

        if(index > buf_end){
            return NULL; // If index exceeds buffer end, return NULL
        }
        
    }
    
        
    return NULL; // If we reach here, global_index was not found
    
    #undef READ_BYTE
    #undef NOT_END
}

/**
 * @brief function to decode the export section on a binary webassembly module.
 * 
 * @param mod pointer to Module State.
 * @return Pointer to module's export if success otherwise NULL.
 *
VecExport * DecodeExportSection(WpModuleState *mod){

    WasmBinSection *sec = &mod->exportsec;
    VecExport *exports = &mod->was->exports;
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

        exports->elements[i].name.lenght = dec_u32;        
        exports->elements[i].name.name = (const char *)index;
        //moving index to import desc
        index = index + dec_u32;

        //Get export description        
        byte_val = READ_BYTE();
        switch (byte_val){
            case 0:                
                exports->elements[i].type = WP_EXTERNAL_TYPE_FUNC;
                break;
            case 1:
                exports->elements[i].type = WP_EXTERNAL_TYPE_TABLE;
                break;
            case 2:
                exports->elements[i].type = WP_EXTERNAL_TYPE_MEMORY;
                break;
            case 3:
                exports->elements[i].type = WP_EXTERNAL_TYPE_GLOBAL;
                break;
            default:
                return NULL;
        }

        //get desc index
        index = DecodeLeb128UInt32(index, &dec_u32);
        if (!index){
            return NULL;                                      
        }
        exports->elements[i].desc.x = dec_u32;
    }
    
    if(index != buf_end){
        //Decode fails       
        return NULL;
    }

    return exports; 
    
    #undef READ_BYTE
    #undef NOT_END
}*/

/**
 * @brief function to get an export by index
 * @param exportsec. Pointer to the WasmBinSection containing the export section.
 * @param export_index index of the export to get
 * @return uint8_t* pointer to the export or NULL if not found
 */
const uint8_t * GetExportByIndex(WasmBinSection exportsec, uint32_t export_index){

    assert(exportsec.content != NULL);                                  // Check if exportsec content is not NULL

    const uint8_t *index = exportsec.content;                           // pointer to byte to traverse the binary file
    const uint8_t *buf_end = index + exportsec.size;                    // pointer to end of section
    uint32_t length;                                                    // auxiliary variable    
    uint32_t dec_u32;  
    
    //get vector length
    index = DecodeLeb128UInt32(index, &length);
    if (!index){        
        return NULL;                                       
    }  
    
   // Check if export_index is valid
    if (export_index >= length) {
        return NULL; // Invalid export index
    }    
      
    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    for(uint32_t i = 0; i < length; i++){

        if(i == export_index){
            return index; // Return pointer to the start of the export type
        }        
        
        //get name's vector
        index = DecodeLeb128UInt32(index, &dec_u32);
        if (!index){
            return NULL;                                      
        }  
        index += dec_u32; // Skip over the name vector

        index++; // skip export description type byte

        if(index > buf_end){
            return NULL; // If index exceeds buffer end, return NULL
        }
        
    }
    return NULL; // If we reach here, export_index was not found
    #undef READ_BYTE
    #undef NOT_END
}

/**
 * @brief function to decode the start section on a binary webassembly module.
 * 
 * @param mod pointer to Module State.
 * @return Pointer to module's start if success otherwise NULL.
 *
uint32_t * DecodeStartSection(WpModuleState *mod){

    WasmBinSection *sec = &mod->startsec;
    uint32_t *start = &mod->was->start;
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
    
}*/


/**
 * @brief function to decode the element section on a binary webassembly module.
 * 
 * @param mod pointer to Module State.
 * @return Pointer to module's elem if success otherwise NULL.
 *
VecElem * DecodeElementSection(WpModuleState *mod){

    WasmBinSection *sec = &mod->elemsec;
    VecElem *elem = &mod->was->elem;
    const uint8_t *index = sec->content;                       // pointer to byte to traverse the binary file
    const uint8_t *buf_end = sec->content + sec->size;                   // pointer to end of binary module
    const uint8_t * start_pos;
    uint32_t element_count;                                            // auxiliary variable   
    uint32_t dec_u32;     
    uint8_t byte_val;
    uint32_t encoded_type;
    uint8_t *inst = NULL;
    

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
                
                elem->elements[i].type = WAS_REFTYPE_FUNCREF;

                //decoding e
                start_pos = index;
                index = SkipExprBuf(index, sec->size);
                if (!index){                    
                    return NULL;                                       
                }
                elem->elements[i].active.offset.end = index;
                elem->elements[i].active.offset.instr = start_pos;

                //decoding vector y* 
                index = DecodeLeb128UInt32(index, &dec_u32);
                if (!index){
                    return NULL;                                       
                }

                //alocating elements on heap
                elem->elements[i].init.elements = ALLOCATE(Expr, dec_u32);
                if (elem->elements[i].init.elements == NULL){        
                    return NULL;                                       
                } 
                elem->elements[i].init.lenght = dec_u32;

                //allocating bytes for each instruction
                inst = ALLOCATE(uint8_t, dec_u32*6);
                if (inst == NULL){        
                    return NULL;                                       
                }
                for (uint32_t ii = 0; ii < dec_u32; ii++)
                {
                    inst[ii*6] = OPCODE_FUNCION_REFERENCE;          //opcode ref.func
                    inst[ii*6+1] = READ_BYTE();                     //byte 1 index
                    inst[ii*6+2] = READ_BYTE();                     //byte 2 index
                    inst[ii*6+3] = READ_BYTE();                     //byte 3 index
                    inst[ii*6+4] = READ_BYTE();                     //byte 4 index
                    inst[ii*6+5] = OPCODE_END;                      //opcode end

                    elem->elements[i].init.elements[ii].instr = inst + ii*6;
                    elem->elements[i].init.elements[ii].end = inst + ii*6 + 6;
                    
                }

                elem->elements[i].mode = WP_WAS_STRUC_MOD_ELEMENT_MODE_ACTIVE;                
                elem->elements[i].active.table_idx = 0;
                elem->elements[i].active.inst=inst;

                          
                break;
            /// 1:u32 et : elemkind 𝑦*:vec(funcidx) ⇒ {type et, init ((ref.func 𝑦) end)*, mode passive}
            case 1:
                
                byte_val = READ_BYTE();     //read elemnt kind
                if(byte_val != 0){
                    //invalid elemkind
                    return NULL; 
                }
                elem->elements[i].type = WAS_REFTYPE_FUNCREF;               

                //decoding vector y* 
                index = DecodeLeb128UInt32(index, &dec_u32);
                if (!index){
                    return NULL;                                       
                }

                //alocating elements on heap
                elem->elements[i].init.elements = ALLOCATE(Expr, dec_u32);
                if (elem->elements[i].init.elements == NULL){        
                    return NULL;                                       
                } 
                elem->elements[i].init.lenght = dec_u32;
                //allocating bytes for each instruction
                inst = ALLOCATE(uint8_t, dec_u32*6);
                if (inst == NULL){        
                    return NULL;                                       
                }
                for (uint32_t ii = 0; ii < dec_u32; ii++)
                {
                    inst[ii*6] = OPCODE_FUNCION_REFERENCE;          //opcode ref.func
                    inst[ii*6+1] = READ_BYTE();                     //byte 1 index
                    inst[ii*6+2] = READ_BYTE();                     //byte 2 index
                    inst[ii*6+3] = READ_BYTE();                     //byte 3 index
                    inst[ii*6+4] = READ_BYTE();                     //byte 4 index
                    inst[ii*6+5] = OPCODE_END;                      //opcode end

                    elem->elements[i].init.elements[ii].instr = inst + ii*6;
                    elem->elements[i].init.elements[ii].end = inst + ii*6 + 6;
                    
                }

                elem->elements[i].mode = WP_WAS_STRUC_MOD_ELEMENT_MODE_PASSIVE;

                elem->elements[i].active.table_idx = 0;
                elem->elements[i].active.offset.end = NULL;
                elem->elements[i].active.offset.instr = NULL;
                elem->elements[i].active.inst=inst;
                
                break;
            //2:u32 𝑥:tableidx 𝑒:expr et : elemkind 𝑦*:vec(funcidx) ⇒ {type et, init ((ref.func 𝑦) end)*, mode active {table 𝑥, offset 𝑒}}
            case 2:
                
                //get table index x
                index = DecodeLeb128UInt32(index, &dec_u32);
                if (!index){
                    return NULL;                                      
                }
                elem->elements[i].active.table_idx = dec_u32;               //𝑥:tableidx

                
                //decoding e
                start_pos = index;
                index = SkipExprBuf(index, sec->size);
                if (!index){                    
                    return NULL;                                       
                }
                elem->elements[i].active.offset.end = index;
                elem->elements[i].active.offset.instr = start_pos;

                //decoding et
                byte_val = READ_BYTE();     //read elemnt kind
                if(byte_val != 0){
                    //invalid elemkind
                    return NULL; 
                }
                elem->elements[i].type = WAS_REFTYPE_FUNCREF; 
                
                //decoding vector y* 
                index = DecodeLeb128UInt32(index, &dec_u32);
                if (!index){
                    return NULL;                                       
                }

                //alocating elements on heap
                elem->elements[i].init.elements = ALLOCATE(Expr, dec_u32);
                if (elem->elements[i].init.elements == NULL){        
                    return NULL;                                       
                } 
                elem->elements[i].init.lenght = dec_u32;

                //allocating bytes for each instruction
                inst = ALLOCATE(uint8_t, dec_u32*6);
                if (inst == NULL){        
                    return NULL;                                       
                }
                for (uint32_t ii = 0; ii < dec_u32; ii++)
                {
                    inst[ii*6] = OPCODE_FUNCION_REFERENCE;          //opcode ref.func
                    inst[ii*6+1] = READ_BYTE();                     //byte 1 index
                    inst[ii*6+2] = READ_BYTE();                     //byte 2 index
                    inst[ii*6+3] = READ_BYTE();                     //byte 3 index
                    inst[ii*6+4] = READ_BYTE();                     //byte 4 index
                    inst[ii*6+5] = OPCODE_END;                      //opcode end

                    elem->elements[i].init.elements[ii].instr = inst + ii*6;
                    elem->elements[i].init.elements[ii].end = inst + ii*6 + 6;
                    
                }

                elem->elements[i].mode = WP_WAS_STRUC_MOD_ELEMENT_MODE_ACTIVE;                
                elem->elements[i].active.table_idx = 0;
                elem->elements[i].active.inst=inst;             
                
                break;
            // 3:u32 et : elemkind 𝑦*:vec(funcidx) ⇒ {type et, init ((ref.func 𝑦) end)*, mode declarative}
            case 3:

                //decoding et
                byte_val = READ_BYTE();     //read elemnt kind
                if(byte_val != 0){
                    //invalid elemkind
                    return NULL; 
                }
                elem->elements[i].type = WAS_REFTYPE_FUNCREF; 
               
                //decoding vector y* 
                index = DecodeLeb128UInt32(index, &dec_u32);
                if (!index){
                    return NULL;                                       
                }

                //alocating elements on heap
                elem->elements[i].init.elements = ALLOCATE(Expr, dec_u32);
                if (elem->elements[i].init.elements == NULL){        
                    return NULL;                                       
                } 
                elem->elements[i].init.lenght = dec_u32;

                //allocating bytes for each instruction
                inst = ALLOCATE(uint8_t, dec_u32*6);
                if (inst == NULL){        
                    return NULL;                                       
                }
                for (uint32_t ii = 0; ii < dec_u32; ii++)
                {
                    inst[ii*6] = OPCODE_FUNCION_REFERENCE;          //opcode ref.func
                    inst[ii*6+1] = READ_BYTE();                     //byte 1 index
                    inst[ii*6+2] = READ_BYTE();                     //byte 2 index
                    inst[ii*6+3] = READ_BYTE();                     //byte 3 index
                    inst[ii*6+4] = READ_BYTE();                     //byte 4 index
                    inst[ii*6+5] = OPCODE_END;                      //opcode end

                    elem->elements[i].init.elements[ii].instr = inst + ii*6;
                    elem->elements[i].init.elements[ii].end = inst + ii*6 + 6;
                    
                }

                elem->elements[i].mode = WP_WAS_STRUC_MOD_ELEMENT_MODE_DECLARATIVE;                
                elem->elements[i].active.table_idx = 0;
                elem->elements[i].active.offset.end = NULL;
                elem->elements[i].active.offset.instr = NULL;
                elem->elements[i].active.inst=inst; 

                break;
            // 4:u32 𝑒:expr el*:vec(expr) ⇒ {type funcref, init el*, mode active {table 0, offset 𝑒}}
            case 4:

                elem->elements[i].type = WAS_REFTYPE_FUNCREF;

                //decoding e
                start_pos = index;
                index = SkipExprBuf(index, sec->size);
                if (!index){                    
                    return NULL;                                       
                }
                elem->elements[i].active.offset.end = index;
                elem->elements[i].active.offset.instr = start_pos;

                //decoding vector el
                index = DecodeLeb128UInt32(index, &dec_u32);
                if (!index){
                    return NULL;                                       
                }

                //alocating elements on heap
                elem->elements[i].init.elements = ALLOCATE(Expr, dec_u32);
                if (elem->elements[i].init.elements == NULL){        
                    return NULL;                                       
                } 
                elem->elements[i].init.lenght = dec_u32;
                for (uint32_t ii = 0; ii < dec_u32; ii++)
                {   
                    start_pos = index;
                    index = SkipExprBuf(index, sec->size);
                    if (!index){                    
                        return NULL;                                       
                    }                    
                    elem->elements[i].init.elements[ii].instr = start_pos;
                    elem->elements[i].init.elements[ii].end = index;
                    
                }

                elem->elements[i].mode = WP_WAS_STRUC_MOD_ELEMENT_MODE_ACTIVE;                
                elem->elements[i].active.table_idx = 0;
                elem->elements[i].active.offset.end = NULL;
                elem->elements[i].active.offset.instr = NULL;
                elem->elements[i].active.inst=NULL;

                break;
            // 5:u32 et : reftype el*:vec(expr) ⇒ {type 𝑒𝑡, init el*, mode passive}
            case 5:

                //decoding et
                byte_val = READ_BYTE();     //read reference type
                if(byte_val != 0x6F || byte_val != 0x70){
                    //invalid reftype
                    return NULL; 
                }
                elem->elements[i].type = byte_val;

                //decoding vector el
                index = DecodeLeb128UInt32(index, &dec_u32);
                if (!index){
                    return NULL;                                       
                }
                //alocating elements on heap
                elem->elements[i].init.elements = ALLOCATE(Expr, dec_u32);
                if (elem->elements[i].init.elements == NULL){        
                    return NULL;                                       
                } 
                elem->elements[i].init.lenght = dec_u32;

                for (uint32_t ii = 0; ii < dec_u32; ii++)
                {   
                    start_pos = index;
                    index = SkipExprBuf(index, sec->size);
                    if (!index){                    
                        return NULL;                                       
                    }                    
                    elem->elements[i].init.elements[ii].instr = start_pos;
                    elem->elements[i].init.elements[ii].end = index;
                    
                }

                elem->elements[i].mode = WP_WAS_STRUC_MOD_ELEMENT_MODE_PASSIVE;                
                elem->elements[i].active.table_idx = 0;
                elem->elements[i].active.offset.end = NULL;
                elem->elements[i].active.offset.instr = NULL;
                elem->elements[i].active.inst=NULL;

                break;
            // 6:u32 𝑥:tableidx 𝑒:expr et : reftype el*:vec(expr) ⇒ {type 𝑒𝑡, init el*, mode active {table 𝑥, offset 𝑒}}
            case 6:

                //get table index x
                index = DecodeLeb128UInt32(index, &dec_u32);
                if (!index){
                    return NULL;                                      
                }
                elem->elements[i].active.table_idx = dec_u32;               //𝑥:tableidx

                //decoding e
                start_pos = index;
                index = SkipExprBuf(index, sec->size);
                if (!index){                    
                    return NULL;                                       
                }
                elem->elements[i].active.offset.end = index;
                elem->elements[i].active.offset.instr = start_pos;

                //decoding et
                byte_val = READ_BYTE();     //read reference type
                if(byte_val != 0x6F || byte_val != 0x70){
                    //invalid reftype
                    return NULL; 
                }
                elem->elements[i].type = byte_val;

                //decoding vector el
                index = DecodeLeb128UInt32(index, &dec_u32);
                if (!index){
                    return NULL;                                       
                }
                //alocating elements on heap
                elem->elements[i].init.elements = ALLOCATE(Expr, dec_u32);
                if (elem->elements[i].init.elements == NULL){        
                    return NULL;                                       
                } 
                elem->elements[i].init.lenght = dec_u32;

                for (uint32_t ii = 0; ii < dec_u32; ii++)
                {   
                    start_pos = index;
                    index = SkipExprBuf(index, sec->size);
                    if (!index){                    
                        return NULL;                                       
                    }                    
                    elem->elements[i].init.elements[ii].instr = start_pos;
                    elem->elements[i].init.elements[ii].end = index;
                    
                }

                elem->elements[i].mode = WP_WAS_STRUC_MOD_ELEMENT_MODE_ACTIVE;  
                elem->elements[i].active.inst=NULL;


                break;
            // 7:u32 et : reftype el*:vec(expr) ⇒ {type 𝑒𝑡, init el*, mode declarative}
            case 7:

                //decoding et
                byte_val = READ_BYTE();     //read reference type
                if(byte_val != 0x6F || byte_val != 0x70){
                    //invalid reftype
                    return NULL; 
                }

                //decoding vector el
                index = DecodeLeb128UInt32(index, &dec_u32);
                if (!index){
                    return NULL;                                       
                }
                //alocating elements on heap
                elem->elements[i].init.elements = ALLOCATE(Expr, dec_u32);
                if (elem->elements[i].init.elements == NULL){        
                    return NULL;                                       
                } 
                elem->elements[i].init.lenght = dec_u32;

                for (uint32_t ii = 0; ii < dec_u32; ii++)
                {   
                    start_pos = index;
                    index = SkipExprBuf(index, sec->size);
                    if (!index){                    
                        return NULL;                                       
                    }                    
                    elem->elements[i].init.elements[ii].instr = start_pos;
                    elem->elements[i].init.elements[ii].end = index;
                    
                }

                elem->elements[i].mode = WP_WAS_STRUC_MOD_ELEMENT_MODE_DECLARATIVE;                
                elem->elements[i].active.table_idx = 0;
                elem->elements[i].active.offset.end = NULL;
                elem->elements[i].active.offset.instr = NULL;
                elem->elements[i].active.inst=NULL;

                break;
                
            default:
                return NULL;
            
        }       
        
    }
    
    if(index != buf_end){
       return NULL;
    }
    return elem;
    
    #undef READ_BYTE
    #undef NOT_END
}*/

/**
 * @brief function to get an element by index
 * @param elemsec. Pointer to the WasmBinSection containing the element section.
 * @param elem_index index of the element to get
 * @return uint8_t* pointer to the element or NULL if not found
 */
const uint8_t * GetElementByIndex(WasmBinSection elemsec, uint32_t elem_index){

    assert(elemsec.content != NULL);                                  // Check if elemsec content is not NULL

    const uint8_t *index = elemsec.content;                           // pointer to byte to traverse the binary file
    const uint8_t *buf_end = index + elemsec.size;                    // pointer to end of section
    uint32_t length;                                                    // auxiliary variable    
    uint32_t vec_len;                                                   // auxiliary variable to decode leb128 values
    uint32_t dec_u32;                                                   // auxiliary variable to decode leb128 values
    uint8_t byte_val;
    
    //get vector length
    index = DecodeLeb128UInt32(index, &length);
    if (!index){        
        return NULL;                                       
    }  
    
   // Check if elem_index is valid
    if (elem_index >= length) {
        return NULL; // Invalid element index
    }
    
      
    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    for(uint32_t i = 0; i < length; i++){

        if(i == elem_index){
            return index; // Return pointer to the start of the element type
        }        
        
        //get type        
        index = DecodeLeb128UInt32(index, &dec_u32);
        if (!index){
            return NULL;                                      
        }
        
        switch(dec_u32) {
            case 0: //0:u32 𝑒:expr 𝑦*:vec(funcidx)
                index = SkipExprBuf(index, buf_end-index);
                if (!index){                    
                    return NULL;                                       
                }
                //get vector y*
                index = DecodeLeb128UInt32(index, &vec_len);        
                if (!index){
                    return NULL;                                      
                }
                for(uint32_t ii = 0; i < vec_len; ii++){
                    index = DecodeLeb128UInt32(index, &dec_u32);
                    if (!index){
                        return NULL;                                      
                    }
                }
                break;
            case 1: //1:u32 et : elemkind 𝑦*:vec(funcidx)
                byte_val = READ_BYTE();     //read elemnt kind
                if(byte_val != 0){
                    //invalid elemkind
                    return NULL; 
                }
                //get vector y*
                index = DecodeLeb128UInt32(index, &vec_len);        
                if (!index){
                    return NULL;                                      
                }
                for(uint32_t ii = 0; i < vec_len; ii++){
                    index = DecodeLeb128UInt32(index, &dec_u32);
                    if (!index){
                        return NULL;                                      
                    }
                }
                break;
            case 2: //2:u32 𝑥:tableidx 𝑒:expr et : elemkind 𝑦*:vec(funcidx)
                index = DecodeLeb128UInt32(index, &dec_u32); // Skip table index and expression
                if (!index){
                    return NULL;                                      
                }
                //get expresion
                index = SkipExprBuf(index, buf_end-index);
                if (!index){                    
                    return NULL;                                       
                }
                //get element kind
                index++; // Skip over the element kind byte
                //get vector y*
                index = DecodeLeb128UInt32(index, &vec_len);        
                if (!index){
                    return NULL;                                      
                }
                for(uint32_t ii = 0; i < vec_len; ii++){
                    index = DecodeLeb128UInt32(index, &dec_u32);
                    if (!index){
                        return NULL;                                      
                    }
                }
                break;
            case 3: //3:u32 et : elemkind 𝑦*:vec(funcidx)
                index++; // Skip over the element kind byte
                //get vector y*
                index = DecodeLeb128UInt32(index, &vec_len);        
                if (!index){
                    return NULL;                                      
                }
                for(uint32_t ii = 0; i < vec_len; ii++){
                    index = DecodeLeb128UInt32(index, &dec_u32);
                    if (!index){
                        return NULL;                                      
                    }
                }
                break;
            case 4: //4:u32 𝑒:expr el*:vec(expr)    
                index = SkipExprBuf(index, buf_end-index);
                if (!index){                    
                    return NULL;                                       
                }
                //get vector el
                index = DecodeLeb128UInt32(index, &vec_len);        
                if (!index){
                    return NULL;                                      
                }
                for(uint32_t ii = 0; i < vec_len; ii++){
                    index = SkipExprBuf(index, buf_end-index);
                    if (!index){                    
                        return NULL;                                       
                    }
                }
                break;
            case 5: //5:u32 et : reftype el*:vec(expr)
                index++; // Skip over the reference type byte
                //get el vector
                index = DecodeLeb128UInt32(index, &vec_len);        
                if (!index){
                    return NULL;                                      
                }
                for(uint32_t ii = 0; i < vec_len; ii++){
                    index = SkipExprBuf(index, buf_end-index);
                    if (!index){                    
                        return NULL;                                       
                    }
                }
                break;
            case 6: //6:u32 𝑥:tableidx 𝑒:expr et : reftype el*:vec(expr)
                index = DecodeLeb128UInt32(index, &dec_u32);    // Skip table index and expression
                //get expresion
                index = SkipExprBuf(index, buf_end-index);
                if (!index){                    
                    return NULL;                                       
                }
                index++; // Skip over the reference type
                //get el
                index = DecodeLeb128UInt32(index, &vec_len);        
                if (!index){
                    return NULL;                                      
                }
                for(uint32_t ii = 0; i < vec_len; ii++){
                    index = SkipExprBuf(index, buf_end-index);
                    if (!index){                    
                        return NULL;                                       
                    }
                }
                break;
            case 7: //7:u32 et : reftype el*:vec(expr)
                index++; // Skip over the reference type byte
                //get el
                index = DecodeLeb128UInt32(index, &vec_len);        
                if (!index){
                    return NULL;                                      
                }
                for(uint32_t ii = 0; i < vec_len; ii++){
                    index = SkipExprBuf(index, buf_end-index);
                    if (!index){                    
                        return NULL;                                       
                    }
                }
                break;
            default:
                return NULL;
        }
        
        if(index >= buf_end){
            return NULL;
        }
    }
    return NULL;
    #undef READ_BYTE
    #undef NOT_END
}


/**
 * @brief function to decode the code section on a binary webassembly module.
 * 
 * @param mod pointer to decoded mod instance. 
 * @return Pointer to module's funcs otherwise NULL.
 *
VecFunc * DecodeCodeSection(WpModuleState *mod){

    WasmBinSection *sec = &mod->codesec;
    Code code_entry;    
    VecFunc *funcs = &mod->was->funcs; 
    const uint8_t *index = sec->content;                                // pointer to byte to traverse the binary file
    const uint8_t *buf_end = sec->content + sec->size;                  // pointer to end of binary module    
    uint32_t code_count; 
    
    //ensure thar funcs is not null
    if(!funcs){
        return NULL;
    }
    
    //get code count
    index = DecodeLeb128UInt32(index, &code_count);
    if (!index){
        return NULL;                                      
    }
    /// check if code count is equal to funcs length
    if(code_count != funcs->lenght){
        return NULL;
    }    
    
    for(uint32_t i = 0; i < code_count; i++){        
        
        //Decoding code rule
        index = DecodeCode(index, &code_entry);        
        if (!index){
            return NULL;                                      
        }
        
        //traversing locals to get total
        uint32_t total_locals = 0;
        for(uint32_t j = 0; j < code_entry.code.locals.lenght; j++){
            total_locals += code_entry.code.locals.elements[j].n;
        } 

        //allocating memory for locals   
        funcs->elements[i].locals.elements = ALLOCATE(Locals, total_locals);
        if (!funcs->elements[i].locals.elements){        
            //return NULL;                                       
        }
        funcs->elements[i].locals.lenght = total_locals;

        //Copying locals
        uint32_t ii = 0;       
        for(uint32_t j = 0; j < code_entry.code.locals.lenght; j++){
            //outer loop for traversing locals vector
            for(uint32_t k = 0; k < code_entry.code.locals.elements[j].n; k++){
                //inner loop for locals number of elements n
                funcs->elements[i].locals.elements[ii].n = code_entry.code.locals.elements[i].n;
                funcs->elements[i].locals.elements[ii].t = code_entry.code.locals.elements[i].t;
                ii++;
            }
        }

        //fucntion body
        funcs->elements[i].body.instr = code_entry.code.e.instr;
        funcs->elements[i].body.end = code_entry.code.e.end;

        //cleaning code_entry for next iteration
        code_entry.size = 0;
        FREE_MEM(code_entry.code.locals.elements);
        code_entry.code.locals.lenght = 0;
        code_entry.code.e.instr = NULL;
        code_entry.code.e.end = NULL;

    }
    
    if(index != buf_end){        
       //return NULL;
    }
    
    return funcs; 
    
}*/

/**
 * @brief function to get a code by index
 * @param codesec. Pointer to the WasmBinSection containing the code section.
 * @param code_index index of the code to get
 * @return uint8_t* pointer to the code or NULL if not found
 */
const uint8_t * GetCodeByIndex(WasmBinSection codesec, uint32_t code_index){

    assert(codesec.content != NULL);                                  // Check if codesec content is not NULL

    const uint8_t *index = codesec.content;                           // pointer to byte to traverse the binary file
    const uint8_t *buf_end = index + codesec.size;                    // pointer to end of section
    uint32_t length;                                                  // auxiliary variable    
    uint32_t dec_u32;                                                // auxiliary variable to decode leb128 values
    
    //get vector length
    index = DecodeLeb128UInt32(index, &length);
    if (!index){        
        return NULL;                                       
    }  
    
   // Check if code_index is valid
    if (code_index >= length) {
        return NULL; // Invalid element index
    }
    
      
    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    for(uint32_t i = 0; i < length; i++){

        if(i == code_index){
            return (uint8_t *)index; // Return pointer to the start of the element type
        }        
        
        //get size of code        
        index = DecodeLeb128UInt32(index, &dec_u32);
        if (!index){
            return NULL;                                      
        }
        
        index = index + dec_u32; // Move index forward by the size of the code
        
        if(index >= buf_end){
            return NULL;
        }
    }

    return NULL;
    #undef READ_BYTE
    #undef NOT_END
}


/**
 * @brief function to decode the data section on a binary webassembly module.
 * 
 * @param mod pointer to Module State.
 * @return Pointer to module's elem if success otherwise NULL.
 *
VecData * DecodeDataSection(WpModuleState *mod){
    
    WasmBinSection *sec = &mod->datasec;
    VecData *data = &mod->was->data;
    const uint8_t *index = sec->content;                       // pointer to byte to traverse the binary file
    const uint8_t *buf_end = sec->content + sec->size;                   // pointer to end of binary module
    const uint8_t *start_pos;
    uint32_t dec_u32;                                         // auxiliary variable     
    uint32_t mode_type;  
    uint8_t byte_val;
    

    //get data count
    index = DecodeLeb128UInt32(index, &dec_u32);
    if (!index){
        return NULL;                                       
    }   
    //alocating elements on heap
    data->elements = ALLOCATE(Data, dec_u32);
    if (!data->elements){        
        return NULL;                                    
    }
    data->length = dec_u32;
    
    
    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    for(uint32_t i = 0; i < data->length; i++){

        //get mode        
        index = DecodeLeb128UInt32(index, &mode_type);
        if (!index){
            return NULL;                                      
        }
        
        switch(mode_type) {
            /// 0:u32 𝑒:expr 𝑏*:vec(byte) ⇒ {init 𝑏*, mode active {memory 0, offset 𝑒}}
            case 0:
                
                //decoding e
                start_pos = index;
                index = SkipExprBuf(index, sec->size);
                if (!index){                    
                    return NULL;                                       
                }
                data->elements[i].active.offset.end = index;
                data->elements[i].active.offset.instr = start_pos;

                //decoding init
                index = DecodeLeb128UInt32(index, &dec_u32);
                if (!index){
                    return NULL;                                     
                }
                data->elements[i].init.lenght = dec_u32;
                data->elements[i].init.elements = index;
                index = index + dec_u32;
                
                data->elements[i].mode = WP_WAS_STRUC_MOD_DATA_MODE_ACTIVE;
                data->elements[i].active.memory = 0;

                break;
            /// 1:u32 𝑏*:vec(byte) ⇒ {init 𝑏*, mode passive}
            case 1:

                //decoding init
                index = DecodeLeb128UInt32(index, &dec_u32);
                if (!index){
                    return NULL;                                     
                }
                data->elements[i].init.lenght = dec_u32;
                data->elements[i].init.elements = index;
                index = index + dec_u32;

                data->elements[i].mode = WP_WAS_STRUC_MOD_DATA_MODE_PASSIVE;
                
                data->elements[i].active.memory = 0;
                data->elements[i].active.offset.end = NULL;
                data->elements[i].active.offset.instr = NULL;                

                break;
            /// 2:u32 𝑥:memidx 𝑒:expr 𝑏*:vec(byte) ⇒ {init 𝑏*, mode active {memory 𝑥, offset 𝑒}}
            case 2:

                
                //get memidx x
                index = DecodeLeb128UInt32(index, &dec_u32);
                if (!index){
                    return NULL;                                     
                }
                data->elements[i].active.memory = dec_u32;               //𝑥:memidx
                //decoding e
                start_pos = index;
                index = SkipExprBuf(index, sec->size);
                if (!index){                    
                    return NULL;                                       
                }
                data->elements[i].active.offset.end = index;
                data->elements[i].active.offset.instr = start_pos;
                //decoding init
                index = DecodeLeb128UInt32(index, &dec_u32);
                if (!index){
                    return NULL;                                     
                }
                data->elements[i].init.lenght = dec_u32;
                data->elements[i].init.elements = index;
                index = index + dec_u32;
                
                data->elements[i].mode = WP_WAS_STRUC_MOD_DATA_MODE_ACTIVE;

                break;           
            default:  
                return NULL;   
        }  
    }
    
    if(index != buf_end){
        return NULL; 
    }

    
    return data; 
    
    #undef READ_BYTE
    #undef NOT_END
}*/

/**
 * @brief function to get a data by index
 * @param datasec. Pointer to the WasmBinSection containing the data section.
 * @param data_index index of the data to get
 * @return uint8_t* pointer to the data or NULL if not found
 */
const uint8_t * GetDataByIndex(WasmBinSection datasec, uint32_t data_index){

    assert(datasec.content != NULL);                                  // Check if datasec content is not NULL

    const uint8_t *index = datasec.content;                           // pointer to byte to traverse the binary file
    const uint8_t *buf_end = index + datasec.size;                    // pointer to end of section
    uint32_t length;                                                  // auxiliary variable    
    uint32_t dec_u32;                                                // auxiliary variable to decode leb128 values
    
    //get vector length
    index = DecodeLeb128UInt32(index, &length);
    if (!index){        
        return NULL;                                       
    }  
    
   // Check if data_index is valid
    if (data_index >= length) {
        return NULL; // Invalid element index
    }
    
      
    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    for(uint32_t i = 0; i < length; i++){

        if(i == data_index){
            return (uint8_t *)index; // Return pointer to the start of the element type
        }        
        
        //get mode        
        index = DecodeLeb128UInt32(index, &dec_u32);
        if (!index){
            return NULL;                                      
        }
        
        switch(dec_u32) {
            case 0: //0:u32 𝑒:expr 𝑏*:vec(byte)
                index = SkipExprBuf(index, buf_end-index);
                if (!index){                    
                    return NULL;                                       
                }
                //get vector b*
                index = DecodeLeb128UInt32(index, &dec_u32);        
                if (!index){
                    return NULL;                                      
                }
                index = index + dec_u32;
                break;
            case 1: //1:u32 𝑏*:vec(byte)
                //get vector b*
                index = DecodeLeb128UInt32(index, &dec_u32);        
                if (!index){
                    return NULL;                                      
                }
                index = index + dec_u32;
                break;
            case 2: //2:u32 𝑥:memidx 𝑒:expr 𝑏*:vec(byte)
                index = DecodeLeb128UInt32(index, &dec_u32); // Skip
                if (!index){
                    return NULL;                                      
                }
                //get expresion
                index = SkipExprBuf(index, buf_end-index);
                if (!index){                    
                    return NULL;                                       
                }
                //get vector b*
                index = DecodeLeb128UInt32(index, &dec_u32);
                if (!index){
                    return NULL;                                      
                }
                index = index + dec_u32;
                break;
        }

        if(index >= buf_end){
            return NULL;
        }
    }
    return NULL;
    #undef READ_BYTE
    #undef NOT_END
}
