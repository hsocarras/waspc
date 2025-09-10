/**
 * @file wasm_validator.c
 * @author H. E. Socarras (hsocarras1987@gmail.com)
 * @brief This file contains the necessary functions to validate the byte buffers in the binary file
 *  corresponding to the different element definitions. 
 * These are utility functions of the validation module but do not belong to the WpValidator class
 * and do not receive the "self" parameter.
 * @version 0.1
 * @date 2024-08-21
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "validation/wasm_validator.h"
#include "validation/wasm_decoder.h"
#include "webassembly/structure/module.h"
#include "webassembly/structure/types.h"
#include "webassembly/binary/module.h"
#include "webassembly/binary/instructions.h"
#include "utils/leb128.h"

#include <stdint.h>
#include <assert.h>
//#include <stdio.h>

/**
 * @brief Function to validate wasm binary magic number
 *          magic ::= 0x00 0x61 0x73 0x6D
 * @param buf wasm binary format 
 * @return uint8_t 1 - ok, 0 - invalid 
 */
uint8_t ValidateMagicBuf(const uint8_t *buf){

   assert(buf != NULL);                                                 // Ensure the buffer is not NULL
    uint8_t magic_number_bytes[4] = {0x00, 0x61, 0x73, 0x6D};           /// Magic number that all wasm file start
    uint32_t wasm_magic_number = *((uint32_t *)magic_number_bytes);     /// magic number in uint32 format to avoid endianess problem
    uint32_t file_magic_number;

    // Magic number check
    file_magic_number = *((uint32_t *)buf);    
    if(file_magic_number != wasm_magic_number){
        
        return 0;
    }
    else return 1;
}

/**
 * @brief Function to validate wasm binary version number
 * 
 * @param buf wasm binary format 
 * @return uint8_t 1 - ok, 0 - invalid 
 */
uint8_t ValidateVersionBuf(const uint8_t *buf, uint32_t *version_number){

    uint8_t version_number_bytes[4] = {0x01, 0x00, 0x00, 0x00};         /// version number    
    uint32_t version_number_1 = *((uint32_t *)version_number_bytes);    /// version number in uint32 format to avoid endianess problem
    uint32_t file_version_number;

    file_version_number = *((uint32_t *)buf); 

    if(file_version_number != version_number_1){
        
        return 0;
    }
    else {

        *version_number = 1;
        return 1;
    } 
}

/**
 * @brief Function to validate function type
 * @param buf  Binary encoded function type
 * @return uint8_t 1 - ok, 0 - invalid
 */
uint8_t ValidateLimitsTypeBuf(const uint8_t *buf, uint32_t k){    

    assert(buf != NULL);                                            // Ensure the buffer is not NULL
    uint32_t min;
    uint32_t max;
    uint8_t is_max_defined;
    const uint8_t *index = buf;                                     // pointer to byte to traverse the binary file

    
    #define READ_BYTE() (*index++)
    is_max_defined = READ_BYTE() & 0x01; // get if max is defined
    if(is_max_defined > 1){
        return 0; // Invalid limits encoding
    }

    index = DecodeLeb128UInt32(index, &min);
    if (!index){
        return 0;                                       
    }
    if(min > k){
        return 0; // Invalid limits
    }
    if(is_max_defined){
        index = DecodeLeb128UInt32(index, &max);
        if (!index){
            return 0;                                       
        }
        if(max < min || max > k){
            return 0; // Invalid limits
        }
    }
    
    return 1; // Valid limits
}

/**
 * @brief Validate Table type
 * The element type must be funcref or externref and the limits must be valid.
 * @param self Pointer to the validator state
 * @param t Pointer to the table to validate   
 * @return uint8_t 0-Invalid, 1-valid
 */
uint8_t ValidateTableTypeBuf(const uint8_t *buf){    

    assert(buf != NULL);                                            // Ensure the buffer is not NULL
    const uint8_t *index = buf;                                     // pointer to byte to traverse the binary file
    ValType encoded_type;

    
    #define READ_BYTE() (*index++)

    encoded_type = READ_BYTE();
    if(encoded_type != WAS_FUNC_REF_TYPE && encoded_type != WAS_EXTERN_REF_TYPE){
        return 0; // Invalid table element type
    }
    if(!ValidateLimitsTypeBuf(index, 0xFFFFFFFF)){
        return 0; // Invalid table limits
    }

    return 1; // Valid table type

    #undef READ_BYTE
}

/**
 * @brief Validate Global type
 * The type must be valid and the mutability must be 0 or 1.
 * @param buf Pointer to the binary encoded global type
 * @return uint8_t 0-Invalid, 1-valid
 */
uint8_t ValidateGlobalTypeBuf(const uint8_t *buf){
    
    assert(buf != NULL);                                            // Ensure the buffer is not NULL
    const uint8_t *index = buf;                                     // pointer to byte to traverse the binary file    
    uint8_t byte_val;

    
    #define READ_BYTE() (*index++)

    byte_val = READ_BYTE(); // get global type
    if(!ValidateValType(byte_val)){
        return 0; // Invalid global type
    }
    byte_val = READ_BYTE(); // get mutability
    if(byte_val > 1){
        return 0; // Invalid global mutability
    }

    return 1; // Valid global type
    #undef READ_BYTE
}

/**
 * @brief Function to validate encoded value type.
 * 
 * @param val_type 
 * @return uint8_t 0-non valid, 1-valid.
 */
uint8_t ValidateValType(uint8_t val_type){
    
    if(val_type >= 0x60 && val_type <= 0x7F){
        if(val_type == 0x60 || val_type == 0x6F || val_type == 0x70 ){            
            return 1;
        }
        if(val_type >= 0x7B && val_type <= 0x7F){            
            return 1;
        }
    }
    else{
        return 0;
    }
    return 0;
}
 
/**
 * @brief Function to validate function type
 * @param buf  Binary encoded function type
 * @return uint8_t 1 - ok, 0 - invalid
 */
uint8_t ValidateFuncTypeBuf(const uint8_t *buf){

    assert(buf != NULL);                                            // Ensure the buffer is not NULL
    const uint8_t *index = buf;                                     // pointer to byte to traverse the binary file
    uint32_t aux_32;
    ValType encoded_type;

    
    #define READ_BYTE() (*index++)

    if(READ_BYTE() != 0x60){ // 0x60 is the opcode for function type
        return 0; // Invalid function type
    }
    
    index = DecodeLeb128UInt32(index, &aux_32);             // get paramateters count
    if (!index){
        return 0; // Invalid leb128 encoding
    }
    for (uint32_t i = 0; i < aux_32; i++){
        if(ValidateValType(READ_BYTE()) == 0){
            return 0; // Invalid value type
        } // Validate each parameter type
    } 
    index = DecodeLeb128UInt32(index, &aux_32);             // get result count
    if (!index){
        return 0; // Invalid leb128 encoding
    }
    for (uint32_t i = 0; i < aux_32; i++){
        if(ValidateValType(READ_BYTE()) == 0){
            return 0; // Invalid value type
        } 
    }

    return 1; // Valid function type

    #undef READ_BYTE
}

/**
 * @brief Validate Import
 * 
 * @param buf Binary encoded import
 * @param functiontype_count Number of function types in the module to validate function type index
 * @return uint8_t 0-Invalid, 1-valid
 */
uint8_t ValidateImportBuf(const uint8_t *buf, uint32_t functiontype_count){

    assert(buf != NULL);                                            // Ensure the buffer is not NULL
    const uint8_t *index = buf;                                     // pointer to byte to traverse the binary file
    uint32_t aux_32;
    uint8_t byte_val;
    ValType encoded_type;

    
    #define READ_BYTE() (*index++)

    index = DecodeLeb128UInt32(index, &aux_32);             // get module name length
    if (!index){
        return 0; // Invalid leb128 encoding
    }
    index = index + aux_32; // skip module name

    index = DecodeLeb128UInt32(index, &aux_32);             // get name length
    if (!index){
        return 0; // Invalid leb128 encoding
    }
    index = index + aux_32; // skip name

    byte_val = READ_BYTE(); // get import desc type
    switch(byte_val){
        case 0: // function
            index = DecodeLeb128UInt32(index, &aux_32);             // get function type index
            if (!index){
                return 0; // Invalid leb128 encoding
            }   
            // Validate function type index
            if(aux_32 >= functiontype_count){
                return 0; // Invalid function type
            }         
            break;
        case 1: // table            
            if(!ValidateTableTypeBuf(index)){
                return 0; // Invalid table type
            }
            break;
        case 2: // memory
            if(!ValidateLimitsTypeBuf(index, 0x010000)){
                return 0; // Invalid memory limits
            }
            break;
        case 3: // global            
            if(!ValidateGlobalTypeBuf(index)){
                return 0; // Invalid global type
            }            
            break;  
        default:
            return 0; // Invalid import desc type
    }

    return 1; // Valid import
    #undef READ_BYTE
}

/**
 * @brief Validate Export
 * @param buf Binary encoded export
 * @param functiontype_count Number of function types in the module to validate function index
 * @param table_count Number of tables in the module to validate table index
 * @param memory_count Number of memories in the module to validate memory index
 * @param global_count Number of globals in the module to validate global index
 * @return uint8_t 0-Invalid, 1-valid
*/
uint8_t ValidateExportBuf(const uint8_t *buf, uint32_t functiontype_count, uint32_t table_count, uint32_t memory_count, uint32_t global_count){

    assert(buf != NULL);                                            // Ensure the buffer is not NULL
    const uint8_t *index = buf;                                     // pointer to byte to traverse the binary file
    uint32_t aux_32;
    uint8_t byte_val;

    
    #define READ_BYTE() (*index++)

    index = DecodeLeb128UInt32(index, &aux_32);                     // get name length
    if (!index){
        return 0; // Invalid leb128 encoding
    }
    index = index + aux_32; // skip name

    byte_val = READ_BYTE(); // get export desc type
    switch(byte_val){
        case 0: // function
            index = DecodeLeb128UInt32(index, &aux_32);             // get function index            
            if (!index){
                return 0; // Invalid leb128 encoding
            }        
            if(aux_32 >= functiontype_count){
                return 0; // Invalid function index
            }               
            break;
        case 1: // table            
            index = DecodeLeb128UInt32(index, &aux_32);             // get table index            
            if (!index){
                return 0; // Invalid leb128 encoding
            }     
            if(aux_32 >= table_count){
                return 0; // Invalid table index
            }                  
            break;
        case 2: // memory
            index = DecodeLeb128UInt32(index, &aux_32);             // get memory index            
            if (!index){
                return 0; // Invalid leb128 encoding
            }      
            if(aux_32 >= memory_count){
                return 0; // Invalid memory index
            }                 
            break;
        case 3: // global            
            index = DecodeLeb128UInt32(index, &aux_32);             // get global index            
            if (!index){
                return 0; // Invalid leb128 encoding
            }
            if(aux_32 >= global_count){
                return 0; // Invalid global index
            }                       
            break;  
        default:
            return 0; // Invalid export desc type
    }

    return 1; // Valid export
    #undef READ_BYTE
}

uint8_t ValidateElementBuf(const uint8_t *buf, uint32_t function_count, uint32_t table_count){

    assert(buf != NULL);                                            // Ensure the buffer is not NULL
    const uint8_t *index = buf;                                     // pointer to byte to traverse the binary file
    uint32_t aux_32;    
    uint8_t byte_val;
    uint32_t idx = 0;   // auxiliary variable to decode index
    
    #define READ_BYTE() (*index++)

    index = DecodeLeb128UInt32(index, &aux_32);             // get element segment index
    if (!index){
        return 0; // Invalid leb128 encoding
    }
    switch (aux_32)    {
    case 0: //0:u32 𝑒:expr 𝑦*:vec(funcidx)
        uint32_t funcidx = 0;   // auxiliary variable to decode function index
        if(!ValidateConstantExprBuf(index, 0xFFFFFFFF)){
            return 0; // Invalid constant expression
        }
        index = SkipExprBuf(index, 0xFFFFFFFF); // Skip expression
        if (!index){                    
            return 0;                                       
        }
        index = DecodeLeb128UInt32(index, &aux_32);        // get vector length
        if (!index){
            return 0; // Invalid leb128 encoding
        }
        for(uint32_t i = 0; i < aux_32; i++){
            index = DecodeLeb128UInt32(index, &idx);   // get function index
            if (!index){
                return 0; // Invalid leb128 encoding
            }
            if(idx >= function_count){
                return 0; // Invalid function index
            } 
        }
        break;   
    case 1: //1:u32 et : elemkind 𝑦*:vec(funcidx)
        byte_val = READ_BYTE(); // get element kind
        if(byte_val != 0){
            return 0; // Invalid element kind
        }
        index = DecodeLeb128UInt32(index, &aux_32);        // get vector length
        if (!index){
            return 0; // Invalid leb128 encoding
        }
        for(uint32_t i = 0; i < aux_32; i++){
            index = DecodeLeb128UInt32(index, &idx);   // get function index
            if (!index){
                return 0; // Invalid leb128 encoding
            }
            if(idx >= function_count){
                return 0; // Invalid function index
            } 
        }
        break;
    case 2: //2:u32 𝑥:tableidx 𝑒:expr et : elemkind 𝑦*:vec(funcidx)
        
        index = DecodeLeb128UInt32(index, &idx);        // get table index
        if (!index){
            return 0; // Invalid leb128 encoding
        }
        if(idx >= table_count){
            return 0; // Invalid table index
        }
        if(!ValidateConstantExprBuf(index, 0xFFFFFFFF)){
            return 0; // Invalid constant expression
        }
        index = SkipExprBuf(index, 0xFFFFFFFF); // Skip expression
        if (!index){                    
            return 0;                                       
        }
        byte_val = READ_BYTE(); // get element kind
        if(byte_val != 0){
            return 0; // Invalid element kind
        }
        index = DecodeLeb128UInt32(index, &aux_32);        // get vector length
        if (!index){    
            return 0; // Invalid leb128 encoding
        }
        for(uint32_t i = 0; i < aux_32; i++){
            index = DecodeLeb128UInt32(index, &idx);   // get function index
            if (!index){
                return 0; // Invalid leb128 encoding
            }
            if(idx >= function_count){
                return 0; // Invalid function index
            } 
        }
        break;
    case 3: //3:u32 et : elemkind 𝑦*:vec(funcidx)
        byte_val = READ_BYTE(); // get element kind
        if(byte_val != 0){
            return 0; // Invalid element kind
        }
        index = DecodeLeb128UInt32(index, &aux_32);        // get vector length
        if (!index){
            return 0; // Invalid leb128 encoding
        }
        for(uint32_t i = 0; i < aux_32; i++){
            index = DecodeLeb128UInt32(index, &idx);   // get function index
            if (!index){
                return 0; // Invalid leb128 encoding
            }
            if(idx >= function_count){
                return 0; // Invalid function index
            } 
        }    
        break;
    case 4: //4:u32 𝑒:expr el*:vec(expr)
        if(!ValidateConstantExprBuf(index, 0xFFFFFFFF)){
            return 0; // Invalid constant expression
        }
        index = SkipExprBuf(index, 0xFFFFFFFF); // Skip expression
        if (!index){                    
            return 0;                                       
        }
        index = DecodeLeb128UInt32(index, &aux_32);        // get vector length
        if (!index){
            return 0; // Invalid leb128 encoding
        }
        for(uint32_t i = 0; i < aux_32; i++){
            if(!ValidateConstantExprBuf(index, 0xFFFFFFFF)){
                return 0; // Invalid constant expression
            }
            index = SkipExprBuf(index, 0xFFFFFFFF); // Skip expression
            if (!index){                    
                return 0;                                       
            }        
        }    
        break;    
    case 5: //5:u32 et : reftype el*:vec(expr)
        byte_val = READ_BYTE(); // get reference type
        if(byte_val != 0x70 && byte_val != 0x6F){
            return 0; // Invalid reference type
        }
        index = DecodeLeb128UInt32(index, &aux_32);        // get vector length
        if (!index){
            return 0; // Invalid leb128 encoding
        }   
        for(uint32_t i = 0; i < aux_32; i++){
            if(!ValidateConstantExprBuf(index, 0xFFFFFFFF)){
                return 0; // Invalid constant expression
            }
            index = SkipExprBuf(index, 0xFFFFFFFF); // Skip expression
            if (!index){                    
                return 0;                                       
            }        
        }
        break;
    case 6: //6:u32 𝑥:tableidx 𝑒:expr et : reftype el*:vec(expr)
        index = DecodeLeb128UInt32(index, &idx);        // get table index
        if (!index){
            return 0; // Invalid leb128 encoding
        }
        if(idx >= table_count){
            return 0; // Invalid table index
        }
        if(!ValidateConstantExprBuf(index, 0xFFFFFFFF)){
            return 0; // Invalid constant expression
        }
        index = SkipExprBuf(index, 0xFFFFFFFF); // Skip expression
        if (!index){                    
            return 0;                                       
        }
        byte_val = READ_BYTE(); // get reference type
        if(byte_val != 0x70 && byte_val != 0x6F){
            return 0; // Invalid reference type
        }
        index = DecodeLeb128UInt32(index, &aux_32);        // get vector length
        if (!index){
            return 0; // Invalid leb128 encoding
        }   
        for(uint32_t i = 0; i < aux_32; i++){
            if(!ValidateConstantExprBuf(index, 0xFFFFFFFF)){
                return 0; // Invalid constant expression
            }
            index = SkipExprBuf(index, 0xFFFFFFFF); // Skip expression
            if (!index){                    
                return 0;                                       
            }        
        }
        break;
    case 7: //7:u32 et : reftype el*:vec(expr)
        byte_val = READ_BYTE(); // get reference type
        if(byte_val != 0x70 && byte_val != 0x6F){
            return 0; // Invalid reference type
        }
        index = DecodeLeb128UInt32(index, &aux_32);        // get vector length
        if (!index){
            return 0; // Invalid leb128 encoding
        }
        for(uint32_t i = 0; i < aux_32; i++){
            if(!ValidateConstantExprBuf(index, 0xFFFFFFFF)){
                return 0; // Invalid constant expression
            }
            index = SkipExprBuf(index, 0xFFFFFFFF); // Skip expression
            if (!index){                    
                return 0;                                       
            }        
        }
        break;
    default:
        return 0; // Invalid element segment index
        break;
    }
    

    return 1; // Valid element segment
    #undef READ_BYTE
}

/**
 * @brief Function to validate constant expression.
 * This function checks if the expression are formed only for constant instructions.
 * @param self Pointer to the validator state
 * @param buf Pointer to the expression to validate
 * @param max_len Maximum length of the expression buffer to avoid endless loops
 * @return uint8_t 1 - ok, 0 - invalid
 */
uint8_t ValidateConstantExprBuf(const uint8_t *buf, uint32_t max_len) {

    assert(buf != NULL);                                            // Ensure the buffer is not NULL

    const uint8_t *index = buf;                                                 // pointer to byte to traverse the binary file
    uint8_t opcode;                                                 // variable to store the opcode 
    uint8_t result;
    uint32_t u32;
    int32_t i32;
    
      
        
    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf + max_len)

    
    while (*index != 0x0B && NOT_END()) {

        opcode = READ_BYTE(); // Read the next opcode

       //check constant opcode
        switch (opcode) {
            case OPCODE_I32_CONST:
                index = DecodeLeb128Int32(index, &i32); // skip i32 value
                if (!index){                
                    return 0;                                       
                }
                break;
            
            //case OPCODE_GLOBAL_GET:
                // Check if global type mut is constant                
                //return 0;
            default:
                return 0; // Invalid opcode in constant expression
        }
    }

    // Check if the expression ended correctly
    if (*index != 0x0B) {
        return 0; // Expression did not end with 0x0B
    }

    return 1;

    #undef READ_BYTE
    #undef NOT_END    
} 

/**
 * @brief Function to validate code buffer.
 * This function checks if the code buffer is valid.
 * It checks the local declarations and the instructions.
 * For simplicity, it only checks for the presence of the end opcode (0x0B).
 * A complete implementation would check each instruction and its operands.
 * @param buf Pointer to the code buffer to validate
 * @return uint8_t 1 - ok, 0 - invalid
 */
uint8_t ValidateCodeBuf(const uint8_t *buf){

    assert(buf != NULL);                                                        // Ensure the buffer is not NULL

    const uint8_t *index = buf;                                                 // pointer to byte to traverse the binary file
    const uint8_t *buf_end;                                                     // pointer to end of section
    uint32_t size;  
    uint32_t local_len;    
    OpCode opcode;
    
    index = DecodeLeb128UInt32(index, &size);                                   // get code size
    if (!index){        
        return 0;                                       
    }
    buf_end = index + size;                                                   // pointer to end of code buffer
    index = DecodeLeb128UInt32(index, &local_len);                            // get local decls length
    if (!index){        
        return 0;                                       
    }

    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    for(uint32_t i = 0; i < local_len; i++){
        uint32_t count;
        ValType type;
        index = DecodeLeb128UInt32(index, &count);                            // get local count
        if (!index){        
            return 0;                                       
        }
        type = READ_BYTE(); // get local type
        if(!ValidateValType(type)){
            return 0; // Invalid value type
        } 
    }

    while (opcode != OPCODE_END && NOT_END()) {
        //look for end opcode
        opcode = READ_BYTE();   // Read the next opcode
        // For simplicity, we will not validate the full instruction set here.
        // A complete implementation would check each opcode and its operands.
    }

    if (opcode != OPCODE_END) {
        return 0; // Expression did not end with 0x0B
    }

    if(index != buf_end){
        return 0; // If index does not point to the end of the buffer, return NULL
    }

    return 1;

    #undef READ_BYTE
    #undef NOT_END    
}

/**
 * @brief Validate Data segment
 * 
 * @param buf Binary encoded data segment
 * @param memory_count Number of memories in the module to validate memory index
 * @return uint8_t 0-Invalid, 1-valid
*/
uint8_t ValidateDataBuf(const uint8_t *buf, uint32_t memory_count){

    assert(buf != NULL);                                            // Ensure the buffer is not NULL

    const uint8_t *index = buf;                                     // pointer to byte to traverse the binary file
    uint32_t aux_32;    
    uint8_t byte_val;
    uint32_t idx = 0;   // auxiliary variable to decode index
    uint32_t data_size = 0;
    
    #define READ_BYTE() (*index++)

    index = DecodeLeb128UInt32(index, &aux_32);                     // get data segment kind
    if (!index){
        return 0; // Invalid leb128 encoding
    }

    switch (aux_32)    {
        case 0: //0:u32 𝑒:expr 𝑏*:vec(byte)
            if(!ValidateConstantExprBuf(index, 0xFFFFFFFF)){
                return 0; // Invalid constant expression
            }
            index = SkipExprBuf(index, 0xFFFFFFFF); // Skip expression
            if (!index){                    
                return 0;                                       
            }
            index = DecodeLeb128UInt32(index, &data_size);        // get data size
            if (!index){
                return 0; // Invalid leb128 encoding
            }
            index = index + data_size; // skip data bytes        
            break;  
        case 1: //1:u32 𝑏*:vec(byte)
            index = DecodeLeb128UInt32(index, &data_size);        // get data size
            if (!index){
                return 0; // Invalid leb128 encoding
            }
            index = index + data_size; // skip data bytes        
            break; 
        case 2: //1:u32 𝑥:memidx 𝑒:expr 𝑏*:vec(byte)            
            index = DecodeLeb128UInt32(index, &idx);        // get memory index
            if (!index){
                return 0; // Invalid leb128 encoding
            }
            if(idx >= memory_count){
                return 0; // Invalid memory index
            }
            if(!ValidateConstantExprBuf(index, 0xFFFFFFFF)){
                return 0; // Invalid constant expression
            }
            index = SkipExprBuf(index, 0xFFFFFFFF); // Skip expression
            if (!index){                    
                return 0;                                       
            }
            index = DecodeLeb128UInt32(index, &data_size);        // get data size
            if (!index){
                return 0; // Invalid leb128 encoding
            }
            index = index + data_size; // skip data bytes        
            break;
    default:
        return 0; // Invalid data segment index
        break;
    }
    return 1; // Valid data segment
    #undef READ_BYTE
}