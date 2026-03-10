/**
 * @file wasm_validator.c
 * @author H. E. Socarras (hsocarras1987@gmail.com)
 * @brief This file contains the necessary functions to validate the byte buffers in the binary file
 * corresponding to the different element definitions. 
 * These are utility functions of the validation module but do not belong to the WpValidator class
 * and do not receive the "self" parameter.
 * They are used to validate the sections of the module and the different types 
 * defined in the binary format ensuring that the buffer complies with binary sintax..
 * @version 0.1
 * @date 2024-08-21
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "validation/wasm_validator.h"
#include "validation/wasm_decoder.h"
#include "webassembly/structure/module.h"
//#include "webassembly/binary/types.h"
#include "webassembly/instructions.h"
#include "utils/leb128.h"

#include <stdint.h>
#include <assert.h>
//#include <stdio.h>


// Sintax validation functions /////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @version 3.0
 * @brief Function to validate wasm binary magic number
 *          magic ::= 0x00 0x61 0x73 0x6D
 * @param buf wasm binary format 
 * @return uint8_t 0 - ok, error code 1 otherwise
 */
uint8_t ValidateMagicBuf(const uint8_t *buf){

   assert(buf != NULL);                                                 // Ensure the buffer is not NULL
    uint8_t magic_number_bytes[4] = {0x00, 0x61, 0x73, 0x6D};           /// Magic number that all wasm file start
    uint32_t wasm_magic_number = *((uint32_t *)magic_number_bytes);     /// magic number in uint32 format to avoid endianess problem
    uint32_t file_magic_number;

    // Magic number check
    file_magic_number = *((uint32_t *)buf);    
    if(file_magic_number != wasm_magic_number){
        
        return 1;
    }
    else return 0;
}

/**
 * @version v3.0
 * @brief Function to validate wasm binary version number
 * @param buf wasm binary format 
 * @param version_number pointer to store the version number
 * @return uint8_t 0 - ok, error code 2 otherwise
 */
uint8_t ValidateVersionBuf(const uint8_t *buf, uint32_t *version_number){

    uint8_t version_number_bytes[4] = {0x01, 0x00, 0x00, 0x00};         /// version number    
    uint32_t version_number_1 = *((uint32_t *)version_number_bytes);    /// version number in uint32 format to avoid endianess problem
    uint32_t file_version_number;

    file_version_number = *((uint32_t *)buf); 

    if(file_version_number != version_number_1){
        
        return 2; // Invalid version number
    }
    else {

        *version_number = 1;
        return 0;
    } 
}

/**
 * @version 3.0
 * @brief Function to validate component type
 * @param buf  Binary encoded component type
 * @return uint8_t 0 - ok, error code otherwise used (3-9)
 * 
 */
uint8_t ValidateCompTypeBuf(const uint8_t *buf){
    const uint8_t *index = buf;
    uint32_t len;
    uint32_t i;
    uint8_t encoded_byte;
    uint8_t mut;

    #define READ_BYTE() (*index++)

    encoded_byte = READ_BYTE();

    switch (encoded_byte)
    {
    case 0x5E:
        encoded_byte = READ_BYTE();
        if(!isStorageType(encoded_byte)){
            return 3; // Invalid storage type
        }
        //mutability byte can or not be present
        return 0;
        break;
    case 0x5F:
        index = DecodeLeb128UInt32(index, &len);  //get field type len for struct
        if(!index){
            return 4; // Invalid leb128 encoding
        }

        for (i = 0; i < len; i++)
        {
            encoded_byte = READ_BYTE();

            if(!isStorageType(encoded_byte)){
                return 5; // Invalid storage type
            }
            //check mutability
            mut = READ_BYTE();
            if(mut != 0x00 || mut != 0x01){
                index--;    //mut byte doesn't present, step back
            }
        }
        return 0; // Valid
        break;
    case 0x60:
        index = DecodeLeb128UInt32(index, &len);             // get paramateters count
        if (!index){
            return 6; // Invalid leb128 encoding
        }

        for (i = 0; i < len; i++){
            if(!IsValType(READ_BYTE())){
                return 7; // Invalid value type
            } 
        } 

        index = DecodeLeb128UInt32(index, &len);             // get result count
        if (!index){
            return 8; // Invalid leb128 encoding
        }

        for (i = 0; i < len; i++){
            if(!IsValType(READ_BYTE())){
                return 9; // Invalid value type
            } 
        }
        return 0; // Valid
        break;
    default:
        return 0;
        break;
    }

    #undef READ_BYTE
}

/**
 * @version 3.0
 * @brief Function to validate subtype
 * @param buf  Binary encoded subtype
 * @param type_count Number of types in the module to validate type indices
 * @return uint8_t 0 - ok, error code otherwise (10-12)
 */
uint8_t ValidateSubtypeBuf(const uint8_t *buf, uint32_t type_count){
    const uint8_t *index = buf;
    uint32_t len;
    uint32_t typeidx;
    uint32_t i;
    uint8_t encoded_byte;

    #define READ_BYTE() (*index++)

    encoded_byte = READ_BYTE();

    switch (encoded_byte)
    {
    case 0x4F: //list final subtype
    case 0x50: //recursive subtype
        index = DecodeLeb128UInt32(index, &len);
        if(!index){
            return 10; // Invalid leb128 encoding
        }

        for (i = 0; i < len; i++)
        {
            index = DecodeLeb128UInt32(index, &typeidx);
            if(!index){
                return 11; // Invalid leb128 encoding
            }
            if(typeidx > type_count){
                return 12; // Invalid index range
            }
        }

        return ValidateCompTypeBuf(index);        
        break;  
    default:
        return ValidateCompTypeBuf(index);
        break;
    }

    #undef READ_BYTE
}

/**
 * @version 3.0
 * @brief Function to validate function type
 * @param buf  Binary encoded function type
 * @return uint8_t 0 - ok,  error code otherwise (13-28)
 */
uint8_t ValidateLimitsTypeBuf(const uint8_t *buf, uint32_t k){    

    assert(buf != NULL);                                            // Ensure the buffer is not NULL

    uint64_t min;
    uint64_t max;
    uint8_t encoded_byte;
    const uint8_t *index = buf;                                     // pointer to byte to traverse the binary file

    
    #define READ_BYTE() (*index++)
    
    encoded_byte = READ_BYTE();
    switch (encoded_byte)
    {
    case 0x00:  //i32 only min
        index = DecodeLeb128UInt64(index, &min);
        if (!index){
            return 13; // Invalid leb128 encoding                                   
        }
        if(min > k){
            return 14; // Invalid limits
        }
        if(min > 0xFFFFFFFF){
            return 15; // Invalid limits
        }
        break;
    case 0x02:  //i32 min and max
        index = DecodeLeb128UInt64(index, &min);
        if (!index){
            return 16;                                       
        }
        if(min > k){
            return 17; // Invalid limits
        }
        if(min > 0xFFFFFFFF){
            return 18; // Invalid limits
        }
        index = DecodeLeb128UInt64(index, &max);
        if (!index){
            return 19;                                       
        }
        if(max < min || max > k){
            return 20; // Invalid limits
        }
        if(max > 0xFFFFFFFF){
            return 21; // Invalid limits
        }
        break;   
    case 0x04:  //i64 only min
        index = DecodeLeb128UInt64(index, &min);
        if (!index){
            return 22;                                       
        }
        if(min > k){
            return 23; // Invalid limits
        }
        break;
    case 0x05:  //i64 min and max
        index = DecodeLeb128UInt64(index, &min);
        if (!index){
            return 24;                                       
        }
        if(min > k){
            return 25; // Invalid limits
        }
        index = DecodeLeb128UInt64(index, &max);
        if (!index){
            return 26;                                       
        }
        if(max < min || max > k){
            return 27; // Invalid limits
        }  
        break;
    default:
        return 28; // Invalid limits encoding
        break;
    }    
    return 0; // Valid limits
}

/**
 * @version 3.0
 * @brief Validate Reference type
 * @param buf Pointer to the binary encoded reference type
 * @return uint8_t 0-ok, error code otherwise (29-31)
 */
uint8_t ValidateRefTypeBuf(const uint8_t *buf){

    assert(buf != NULL);                                            // Ensure the buffer is not NULL
    const uint8_t *index = buf;                                     // pointer to byte to traverse the binary file
    uint8_t encoded_type;
    int64_t aux_64;

    
    #define READ_BYTE() (*index++)

    encoded_type = READ_BYTE();
    switch (encoded_type)
    {
    case 0x63:  // ref null headtype
    case 0x64:  // ref head type        
        if(IsAbsHeadType(encoded_type)){
            break;
        }
        else{
            index = DecodeLeb128Int64(index, &aux_64); // get s33
            if (!index){
                return 29; // Invalid leb128 encoding
            }
            if(aux_64 < -0x100000000  || aux_64 > 0x100000000){
                return 30; // Invalid table limits
            }
        }
        break;
    default:
        if(!IsAbsHeadType(encoded_type)){
            return 31; // Invalid reference type
        }
    }

    return 0; // Valid reference type

    #undef READ_BYTE
}

/**
 * @version 3.0
 * @brief Validate Memory type
 * The limits must be valid.
 * @param buf Pointer to the binary encoded memory type
 * @return uint8_t 0-ok, error code otherwise 
 */
uint8_t ValidateMemTypeBuf(const uint8_t *buf){    

    return ValidateLimitsTypeBuf(buf, 0x010000); // 2^16 = 65536
} //max 65536 pages

/**
 * @version 3.0
 * @brief Validate tag type
 * The tag type must be 0x00 and the type index must be valid.
 * @param buf Pointer to the binary encoded tag type    
 * @return uint8_t 0-ok, error code otherwise (32-33)
 */
uint8_t ValidateTagTypeBuf(const uint8_t *buf){

    assert(buf != NULL);                                            // Ensure the buffer is not NULL
    const uint8_t *index = buf;                                     // pointer to byte to traverse the binary file
    uint8_t encoded_type;
    uint32_t idx;


    
    #define READ_BYTE() (*index++)

    encoded_type = READ_BYTE();

    if(encoded_type != 0x00){
        return 32; // Invalid tag type
    } 
    
    index = DecodeLeb128UInt32(index, &idx);    //get type index
    if(!index){
        return 33; // Invalid leb128 encoding
    }

    return 0; // Valid tag type
    #undef READ_BYTE
}

/**
 * @version 3.0
 * @brief Validate External type
 * @param buf Pointer to the binary encoded external type
 * @return uint8_t 0-ok, error code otherwise (34-38)
 */
uint8_t ValidateExternalTypeBuf(const uint8_t *buf){

    assert(buf != NULL);                                            // Ensure the buffer is not NULL
    const uint8_t *index = buf;                                     // pointer to byte to traverse the binary file
    uint8_t encoded_type;
    uint32_t idx;
    uint8_t err_code;
    
    #define READ_BYTE() (*index++)

    encoded_type = READ_BYTE();

    switch (encoded_type)
    {
    case 0x00:  // function
       index = DecodeLeb128UInt32(index, &idx);             // get function type index  
        if (!index){
            return 34; // Invalid leb128 encoding
        }      
        break;
    case 0x01:  // tableref
        err_code = ValidateTableTypeBuf(index);
        if(err_code > 0){
            return err_code; // Invalid table type
        }
        break;
    case 0x02:  // memoryref
        err_code = ValidateMemTypeBuf(index);
        if(err_code > 0){
            return err_code; // Invalid memory type
        }
        break;
    case 0x03:  // globalref
        err_code = ValidateGlobalTypeBuf(index);
        if(err_code > 0){
            return err_code; // Invalid global type
        }
        break;   
    case 0x04:  // tagtype
        err_code = ValidateTagTypeBuf(index);
        if(err_code > 0){
            return err_code; // Invalid tag type
        }
        break; 
    default:
        return 38; // Invalid external type
        break;
    }
    return 0; // Valid external type
    #undef READ_BYTE
}   

/**
 * @version 3.0
 * @brief Validate Table type
 * The element type must be funcref or externref and the limits must be valid.
 * @param self Pointer to the validator state
 * @param t Pointer to the table to validate   
 * @return uint8_t 0-ok, error code otherwise (39-40)
 */
uint8_t ValidateTableTypeBuf(const uint8_t *buf){    

    assert(buf != NULL);                                            // Ensure the buffer is not NULL
    const uint8_t *index = buf;                                     // pointer to byte to traverse the binary file
    int64_t aux_64;
    uint8_t encoded_type;
    uint8_t err_code;
    
    #define READ_BYTE() (*index++)

    encoded_type = READ_BYTE();
    err_code = ValidateRefTypeBuf(index);
    if(err_code > 0){
        return err_code; // Invalid reference type
    }
    index = SkipRefTypeBuf(index);
    if(!index){
        return 39; // Invalid reference type
    }
    err_code = ValidateLimitsTypeBuf(index, 0xFFFFFFFF);
    if(err_code > 0){
        return err_code; // Invalid table limits
    }

    return 0; // Valid table type

    #undef READ_BYTE
}

/**
 * 
 * @version 3.0
 * @brief TODO doc
 * @param buf Pointer to the binary encoded table type
 * @return uint8_t 0-ok, error code otherwise (41-45)
 */
uint8_t ValidateTableBuf(const uint8_t *buf){
    assert(buf != NULL);                                            // Ensure the buffer is not NULL
    const uint8_t *index = buf;                                     // pointer to byte to traverse the binary file
    uint8_t encoded_type;
    uint8_t err_code;
    
    #define READ_BYTE() (*index++)

    encoded_type = READ_BYTE();
    switch (encoded_type)
    {
    case 0x40:
        encoded_type = READ_BYTE(); //get 0x00 byte
        if(encoded_type != 0x00){
            return 41; // Invalid table type
        }
        err_code = ValidateTableTypeBuf(index);
        if(err_code > 0){
            return err_code; // Invalid table type
        }

        index = SkipTableTypeBuf(index);
        if(!index){
            return 42; // Invalid table type
        }
        //TODO validate expression for table init
        return 0;        
        break;  
    case 0x63:
        //TODO
        return 43;
        break;
    case 0x64:
        //TODO
        return 44;
        break;
    default:
        if(IsAbsHeadType(encoded_type)){
            return 0;
        }
        else{
            return 45; // Invalid table type
        }
        break;
    }


    #undef READ_BYTE
    
}

/**
 * @version 3.0
 * @brief Validate Global type
 * The type must be valid and the mutability must be 0 or 1.
 * @param buf Pointer to the binary encoded global type
 * @return uint8_t 0 - ok, error code otherwise (46-47)
 */
uint8_t ValidateGlobalTypeBuf(const uint8_t *buf){
    
    assert(buf != NULL);                                            // Ensure the buffer is not NULL
    const uint8_t *index = buf;                                     // pointer to byte to traverse the binary file    
    uint8_t byte_val;

    
    #define READ_BYTE() (*index++)

    byte_val = READ_BYTE(); // get global type
    if(!IsValType(byte_val)){
        return 46; // Invalid global type
    }   

    return 0; // Valid global type
    #undef READ_BYTE
}

/**
 * @version 3.0
 * @brief Function to validate recursive type 5.3.8
 * @param buf  Binary encoded function type
 * @return uint8_t 0 - no error, error code otherwise (48-49)
 * @example
 *   uint8_t binary_data[] = {0x60, 0x00, 0x01, 0x7F}; // Example binary data
 *   uint8_t result = ValidateTypeBuf(binary_data, 1);
 *   if (result == 0) {
 *       printf("Invalid type\n");
 *   } else {
 *       printf("Valid type\n");
 *   }
 */
uint8_t ValidateTypeBuf(const uint8_t *buf, uint32_t type_count){

    assert(buf != NULL);                                            // Ensure the buffer is not NULL
    const uint8_t *index = buf;                                     // pointer to byte to traverse the binary file
    uint32_t len;
    uint8_t encoded_type;
    uint8_t err_code;

    
    #define READ_BYTE() (*index++)

    encoded_type = READ_BYTE();
    if(!IsRecType(encoded_type)){ 
        return 48; // Invalid recursive type
    }

    if(encoded_type == 0x4E){ //0x4E st*:list(subtype)
        index = DecodeLeb128UInt32(index, &len);          // get subtypes list's len
        if(!index){
            return 49; // Invalid leb128 encoding
        }
        for (uint32_t i = 0; i < len; i++)
        {
            index = GetSubTypeByIndex(index, i); // get subtype index
            err_code = ValidateSubtypeBuf(index, type_count);
            if(err_code > 0){
                return err_code; // Invalid subtype
            }
        }
        return 0;
    }
    else{
        index--;
        err_code = ValidateSubtypeBuf(index, type_count);   
        if(err_code > 0){
            return err_code; // Invalid subtype
        }
        return 0; // Valid recursive type     
    }       

    #undef READ_BYTE
}

/**
 * @version 3.0
 * @brief Validate Import
 * @param buf Binary encoded import
 * @param functiontype_count Number of function types in the module to validate function type index
 * @return uint8_t 0- no error, error code otherwise (50-52)
 */
uint8_t ValidateImportBuf(const uint8_t *buf, uint32_t type_count){

    assert(buf != NULL);                                            // Ensure the buffer is not NULL
    const uint8_t *index = buf;                                     // pointer to byte to traverse the binary file
    uint32_t aux_32;
    uint8_t byte_val;
    uint8_t err_code;

    
    #define READ_BYTE() (*index++)

    index = DecodeLeb128UInt32(index, &aux_32);             // get module name length
    if (!index){
        return 50; // Invalid leb128 encoding
    }
    index = index + aux_32; // skip module name

    index = DecodeLeb128UInt32(index, &aux_32);             // get name length
    if (!index){
        return 51; // Invalid leb128 encoding
    }
    index = index + aux_32; // skip name
    err_code = ValidateExternalTypeBuf(index);
    if(err_code > 0){
        return err_code; // Invalid external type
    }   

    return 0; // Valid import
    #undef READ_BYTE
}

/**
 * @brief Validate Export
 * @param buf Binary encoded export
 * @param functiontype_count Number of function types in the module to validate function index
 * @param table_count Number of tables in the module to validate table index
 * @param memory_count Number of memories in the module to validate memory index
 * @param global_count Number of globals in the module to validate global index
 * @return uint8_t 0-no error, error code otherwise (53-65)
*/
uint8_t ValidateExportBuf(const uint8_t *buf, uint32_t functiontype_count, uint32_t table_count, uint32_t memory_count, uint32_t global_count){

    assert(buf != NULL);                                            // Ensure the buffer is not NULL
    const uint8_t *index = buf;                                     // pointer to byte to traverse the binary file
    uint32_t aux_32;
    uint8_t byte_val;
    uint8_t err_code;
    
    #define READ_BYTE() (*index++)

    index = DecodeLeb128UInt32(index, &aux_32);                     // get name length
    if (!index){
        return 53; // Invalid leb128 encoding
    }
    index = index + aux_32; // skip name

    byte_val = READ_BYTE(); // get export desc type
    switch(byte_val){
        case 0: // function
            index = DecodeLeb128UInt32(index, &aux_32);             // get function index            
            if (!index){
                return 57; // Invalid leb128 encoding
            }        
            if(aux_32 >= functiontype_count){
                return 58; // Invalid function index
            }               
            break;
        case 1: // table            
            index = DecodeLeb128UInt32(index, &aux_32);             // get table index            
            if (!index){
                return 59; // Invalid leb128 encoding
            }     
            if(aux_32 >= table_count){
                return 60; // Invalid table index
            }                  
            break;
        case 2: // memory
            index = DecodeLeb128UInt32(index, &aux_32);             // get memory index            
            if (!index){
                return 61; // Invalid leb128 encoding
            }      
            if(aux_32 >= memory_count){
                return 62; // Invalid memory index
            }                 
            break;
        case 3: // global            
            index = DecodeLeb128UInt32(index, &aux_32);             // get global index            
            if (!index){
                return 63; // Invalid leb128 encoding
            }
            if(aux_32 >= global_count){
                return 64; // Invalid global index
            }                       
            break;  
        default:
            return 65; // Invalid export desc type
    }

    return 0; // Valid export
    #undef READ_BYTE
}

/**
 * @version 3.0
 * @brief TODO
 * @param buf Pointer to the binary encoded element segment
 * @param function_count Number of functions in the module to validate function indices
 * @param table_count Number of tables in the module to validate table indices
 * @return uint8_t 0-ok, error code otherwise (66-102)
 */
uint8_t ValidateElementBuf(const uint8_t *buf, uint32_t function_count, uint32_t table_count){

    assert(buf != NULL);                                            // Ensure the buffer is not NULL
    const uint8_t *index = buf;                                     // pointer to byte to traverse the binary file
    uint32_t aux_32;    
    uint8_t byte_val;
    uint32_t idx = 0;   // auxiliary variable to decode index
    uint8_t err_code;
    
    #define READ_BYTE() (*index++)

    index = DecodeLeb128UInt32(index, &aux_32);             // get element segment index
    if (!index){
        return 66; // Invalid leb128 encoding
    }
    switch (aux_32)    {
    case 0: //0:u32 𝑒:expr 𝑦*:vec(funcidx)
        uint32_t funcidx = 0;   // auxiliary variable to decode function index
        err_code = ValidateConstantExprBuf(index, 0xFFFFFFFF);
        if(err_code > 0){
            return err_code; // Invalid constant expression
        }
        index = SkipExprBuf(index, 0xFFFFFFFF); // Skip expression
        if (!index){                    
            return 67;                                       
        }
        index = DecodeLeb128UInt32(index, &aux_32);        // get vector length
        if (!index){
            return 68; // Invalid leb128 encoding
        }
        for(uint32_t i = 0; i < aux_32; i++){
            index = DecodeLeb128UInt32(index, &idx);   // get function index
            if (!index){
                return 69; // Invalid leb128 encoding
            }
            if(idx >= function_count){
                return 70; // Invalid function index
            } 
        }
        break;   
    case 1: //1:u32 et : elemkind 𝑦*:vec(funcidx)
        byte_val = READ_BYTE(); // get element kind
        if(byte_val != 0){
            return 71; // Invalid element kind
        }
        index = DecodeLeb128UInt32(index, &aux_32);        // get vector length
        if (!index){
            return 72; // Invalid leb128 encoding
        }
        for(uint32_t i = 0; i < aux_32; i++){
            index = DecodeLeb128UInt32(index, &idx);   // get function index
            if (!index){
                return 73; // Invalid leb128 encoding
            }
            if(idx >= function_count){
                return 74; // Invalid function index
            } 
        }
        break;
    case 2: //2:u32 𝑥:tableidx 𝑒:expr et : elemkind 𝑦*:vec(funcidx)
        
        index = DecodeLeb128UInt32(index, &idx);        // get table index
        if (!index){
            return 75; // Invalid leb128 encoding
        }
        if(idx >= table_count){
            return 76; // Invalid table index
        }
        err_code = ValidateConstantExprBuf(index, 0xFFFFFFFF);
        if(err_code > 0){
            return err_code; // Invalid constant expression
        }
        index = SkipExprBuf(index, 0xFFFFFFFF); // Skip expression
        if (!index){                    
            return 77;                                       
        }
        byte_val = READ_BYTE(); // get element kind
        if(byte_val != 0){
            return 78; // Invalid element kind
        }
        index = DecodeLeb128UInt32(index, &aux_32);        // get vector length
        if (!index){    
            return 79; // Invalid leb128 encoding
        }
        for(uint32_t i = 0; i < aux_32; i++){
            index = DecodeLeb128UInt32(index, &idx);   // get function index
            if (!index){
                return 80; // Invalid leb128 encoding
            }
            if(idx >= function_count){
                return 81; // Invalid function index
            } 
        }
        break;
    case 3: //3:u32 et : elemkind 𝑦*:vec(funcidx)
        byte_val = READ_BYTE(); // get element kind
        if(byte_val != 0){
            return 82; // Invalid element kind
        }
        index = DecodeLeb128UInt32(index, &aux_32);        // get vector length
        if (!index){
            return 83; // Invalid leb128 encoding
        }
        for(uint32_t i = 0; i < aux_32; i++){
            index = DecodeLeb128UInt32(index, &idx);   // get function index
            if (!index){
                return 84; // Invalid leb128 encoding
            }
            if(idx >= function_count){
                return 85; // Invalid function index
            } 
        }    
        break;
    case 4: //4:u32 𝑒:expr el*:vec(expr)
        err_code = ValidateConstantExprBuf(index, 0xFFFFFFFF);
        if(err_code > 0){
            return err_code; // Invalid constant expression
        }
        index = SkipExprBuf(index, 0xFFFFFFFF); // Skip expression
        if (!index){                    
            return 86;                                       
        }
        index = DecodeLeb128UInt32(index, &aux_32);        // get vector length
        if (!index){
            return 87; // Invalid leb128 encoding
        }
        for(uint32_t i = 0; i < aux_32; i++){
            err_code = ValidateConstantExprBuf(index, 0xFFFFFFFF);
            if(err_code > 0){
                return err_code; // Invalid constant expression
            }
            index = SkipExprBuf(index, 0xFFFFFFFF); // Skip expression
            if (!index){                    
                return 88;                                       
            }        
        }    
        break;    
    case 5: //5:u32 et : reftype el*:vec(expr)
        byte_val = READ_BYTE(); // get reference type
        if(byte_val != 0x70 && byte_val != 0x6F){
            return 89; // Invalid reference type
        }
        index = DecodeLeb128UInt32(index, &aux_32);        // get vector length
        if (!index){
            return 90; // Invalid leb128 encoding
        }   
        for(uint32_t i = 0; i < aux_32; i++){
            err_code = ValidateConstantExprBuf(index, 0xFFFFFFFF);
            if(err_code > 0){
                return err_code; // Invalid constant expression
            }
            index = SkipExprBuf(index, 0xFFFFFFFF); // Skip expression
            if (!index){                    
                return 91;                                       
            }        
        }
        break;
    case 6: //6:u32 𝑥:tableidx 𝑒:expr et : reftype el*:vec(expr)
        index = DecodeLeb128UInt32(index, &idx);        // get table index
        if (!index){
            return 92; // Invalid leb128 encoding
        }
        if(idx >= table_count){
            return 93; // Invalid table index
        }
        err_code = ValidateConstantExprBuf(index, 0xFFFFFFFF);
        if(err_code > 0){
            return err_code; // Invalid constant expression
        }
        index = SkipExprBuf(index, 0xFFFFFFFF); // Skip expression
        if (!index){                    
            return 94;                                       
        }
        byte_val = READ_BYTE(); // get reference type
        if(byte_val != 0x70 && byte_val != 0x6F){
            return 95; // Invalid reference type
        }
        index = DecodeLeb128UInt32(index, &aux_32);        // get vector length
        if (!index){
            return 96; // Invalid leb128 encoding
        }   
        for(uint32_t i = 0; i < aux_32; i++){
            err_code = ValidateConstantExprBuf(index, 0xFFFFFFFF);
            if(err_code > 0){
                return err_code; // Invalid constant expression
            }
            index = SkipExprBuf(index, 0xFFFFFFFF); // Skip expression
            if (!index){                    
                return 97;                                       
            }        
        }
        break;
    case 7: //7:u32 et : reftype el*:vec(expr)
        byte_val = READ_BYTE(); // get reference type
        if(byte_val != 0x70 && byte_val != 0x6F){
            return 98; // Invalid reference type
        }
        index = DecodeLeb128UInt32(index, &aux_32);        // get vector length
        if (!index){
            return 99; // Invalid leb128 encoding
        }
        for(uint32_t i = 0; i < aux_32; i++){
            err_code = ValidateConstantExprBuf(index, 0xFFFFFFFF);
            if(err_code > 0){
                return err_code; // Invalid constant expression
            }
            index = SkipExprBuf(index, 0xFFFFFFFF); // Skip expression
            if (!index){                    
                return 100;                                       
            }        
        }
        break;
    default:
        return 101; // Invalid element segment index
        break;
    }
    

    return 0; // Valid element segment
    #undef READ_BYTE
}

/**
 * @brief Function to validate constant expression.
 * This function checks if the expression are formed only for constant instructions.
 * @param self Pointer to the validator state
 * @param buf Pointer to the expression to validate
 * @param max_len Maximum length of the expression buffer to avoid endless loops
 * @return uint8_t 0 - ok, error code otherwise (103-105)
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
                    return 103;                                       
                }
                break;
            
            //case OPCODE_GLOBAL_GET:
                // Check if global type mut is constant                
                //return 0;
            default:
                return 104; // Invalid opcode in constant expression
        }
    }

    // Check if the expression ended correctly
    if (*index != 0x0B) {
        return 105; // Expression did not end with 0x0B
    }

    return 0;

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
 * @return uint8_t 0 - ok, error code otherwise (106-111)
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
        return 106;                                       
    }
    buf_end = index + size;                                                   // pointer to end of code buffer
    index = DecodeLeb128UInt32(index, &local_len);                            // get local decls length
    if (!index){        
        return 107;                                       
    }

    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    for(uint32_t i = 0; i < local_len; i++){
        uint32_t count;
        uint8_t type;
        index = DecodeLeb128UInt32(index, &count);                            // get local count
        if (!index){        
            return 108;                                       
        }
        type = READ_BYTE(); // get local type
        if(!IsValType(type)){
            return 109; // Invalid value type
        } 
    }

    while (opcode != OPCODE_END && NOT_END()) {
        //look for end opcode
        opcode = READ_BYTE();   // Read the next opcode
        // For simplicity, we will not validate the full instruction set here.
        // A complete implementation would check each opcode and its operands.
    }

    if (opcode != OPCODE_END) {
        return 110; // Expression did not end with 0x0B
    }

    if(index != buf_end){
        return 111; // If index does not point to the end of the buffer, return NULL
    }

    return 0;

    #undef READ_BYTE
    #undef NOT_END    
}

/**
 * @brief Validate Data segment
 * 
 * @param buf Binary encoded data segment
 * @param memory_count Number of memories in the module to validate memory index
 * @return uint8_t 0-ok, error code otherwise (112-120)
*/
uint8_t ValidateDataBuf(const uint8_t *buf, uint32_t memory_count){

    assert(buf != NULL);                                            // Ensure the buffer is not NULL

    const uint8_t *index = buf;                                     // pointer to byte to traverse the binary file
    uint32_t aux_32;    
    uint8_t byte_val;
    uint32_t idx = 0;   // auxiliary variable to decode index
    uint32_t data_size = 0;
    uint8_t err_code;
    
    #define READ_BYTE() (*index++)

    index = DecodeLeb128UInt32(index, &aux_32);                     // get data segment kind
    if (!index){
        return 112; // Invalid leb128 encoding
    }

    switch (aux_32)    {
        case 0: //0:u32 𝑒:expr 𝑏*:vec(byte)
            err_code = ValidateConstantExprBuf(index, 0xFFFFFFFF);
            if(err_code > 0){
                return err_code; // Invalid constant expression
            }
            index = SkipExprBuf(index, 0xFFFFFFFF); // Skip expression
            if (!index){                    
                return 113;                                       
            }
            index = DecodeLeb128UInt32(index, &data_size);        // get data size
            if (!index){
                return 114; // Invalid leb128 encoding
            }
            index = index + data_size; // skip data bytes        
            break;  
        case 1: //1:u32 𝑏*:vec(byte)
            index = DecodeLeb128UInt32(index, &data_size);        // get data size
            if (!index){
                return 115; // Invalid leb128 encoding
            }
            index = index + data_size; // skip data bytes        
            break; 
        case 2: //1:u32 𝑥:memidx 𝑒:expr 𝑏*:vec(byte)            
            index = DecodeLeb128UInt32(index, &idx);        // get memory index
            if (!index){
                return 116; // Invalid leb128 encoding
            }
            if(idx >= memory_count){
                return 117; // Invalid memory index
            }
            err_code = ValidateConstantExprBuf(index, 0xFFFFFFFF);
            if(err_code > 0){
                return err_code; // Invalid constant expression
            }
            index = SkipExprBuf(index, 0xFFFFFFFF); // Skip expression
            if (!index){                    
                return 118;                                       
            }
            index = DecodeLeb128UInt32(index, &data_size);        // get data size
            if (!index){
                return 119; // Invalid leb128 encoding
            }
            index = index + data_size; // skip data bytes        
            break;
    default:
        return 0; // Invalid data segment index
        break;
    }
    return 0; // Valid data segment
    #undef READ_BYTE
}

