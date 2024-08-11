/**
 * @file loader.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-06-29
 * 
 * @copyright Copyright (c) 2024
 * 
 */



//standard includes
#include <stdint.h>



///Function to validate wasm binary magic number
//LoaderResult ValidateMagic (uint8_t *, size_t);
/// Function to validate wasm binary version number
//LoaderResult ValidateMagic (uint8_t *, size_t);
/// function to  get a binary module struct from raw byte pointer
//BinaryModule LoadBinaryModule(uint8_t *, size_t );



// [TODO]
/*LoaderResult ValidateBinaryModule( BinaryModule mod){

}*/

/**
 * @brief function that initialise a Binary Module from an file loaded in LOAD_MEMORY
 * 
 * @param wasm Pointer to where binary content start
 * @param len Size in bytes for binary content
 * @return BinaryModule 
 *
BinaryModule LoadBinaryModule(uint8_t *wasm, size_t len){    

    uint32_t var_u32;               // local variable to store a 32 bits unsigner
    uint8_t var_u8;                 // local variable to store a byte 
    uint8_t section_counter = 0;    // Secction counter to make finite loops
    uint8_t *index = wasm;          // pointer to byte to traverse the binary file

    DEC_UINT32_LEB128 wasm_ui32;    //variable to store decoded leb128 u32

    BinaryModule bin_mod = NULL_BINARY_MODULE;
    

    #define READ_BYTE() (*index++)
    #define NOT_END() (index < wasm + len)

    // Magic number check
    if(ValidateMagic(index, 4) == ERR_MAGIC){
        bin_mod.load_status = ERR_MAGIC;
        return bin_mod;
    }
    index = index + 4;

    // Version number check  
    if(ValidateVersion(index, 4) == ERR_VERSION){
        bin_mod.load_status = ERR_VERSION;
        return bin_mod;
    }
    index = index + 4;

    //LOOP to traverse the binary file
    while(NOT_END() && section_counter <= 12){
        //Seccion
        var_u8 = READ_BYTE();
        
        switch(var_u8){
            case 0:{
                //custom section start
                bin_mod.custom_section = index - 1;
                wasm_ui32 = DecodeLeb128UInt32(index);
                bin_mod.custom_section_len = wasm_ui32.value;
                index = index + wasm_ui32.value + wasm_ui32.len;
                break;
            }
            case 1:{
                // Type Section
                bin_mod.type_section = index - 1;
                wasm_ui32 = DecodeLeb128UInt32(index);
                bin_mod.type_section_len = wasm_ui32.value;
                index = index + wasm_ui32.value + wasm_ui32.len;
                break;
            }
            case 2:{
                // import Section
                bin_mod.import_section = index - 1;
                wasm_ui32 = DecodeLeb128UInt32(index);
                bin_mod.import_section_len = wasm_ui32.value;
                index = index + wasm_ui32.value + wasm_ui32.len;
                break;
            }
            case 3:{
                // Function Section
                bin_mod.function_section = index - 1;
                wasm_ui32 = DecodeLeb128UInt32(index);
                bin_mod.function_section_len = wasm_ui32.value;
                index = index + wasm_ui32.value + wasm_ui32.len;
                break;
            }
            case 4:{
                // Table Section
                bin_mod.table_section = index - 1;
                wasm_ui32 = DecodeLeb128UInt32(index);
                bin_mod.table_section_len = wasm_ui32.value;
                index = index + wasm_ui32.value + wasm_ui32.len;
                break;
            }
            case 5:{
                // Memory Section
                bin_mod.memory_section = index - 1;
                wasm_ui32 = DecodeLeb128UInt32(index);
                bin_mod.memory_section_len = wasm_ui32.value;
                index = index + wasm_ui32.value + wasm_ui32.len;
                break;
            }
            case 6:{
                // Function Section
                bin_mod.global_section = index - 1;
                wasm_ui32 = DecodeLeb128UInt32(index);
                bin_mod.global_section_len = wasm_ui32.value;
                index = index + wasm_ui32.value + wasm_ui32.len;
                break;
            }
            case 7:{
                //Export Section
                bin_mod.export_section = index - 1;
                wasm_ui32 = DecodeLeb128UInt32(index);
                bin_mod.export_section_len = wasm_ui32.value;
                index = index + wasm_ui32.value + wasm_ui32.len;
                break;
            }
            case 8:{
                // Start Section
                bin_mod.start_section = index - 1;
                wasm_ui32 = DecodeLeb128UInt32(index);
                bin_mod.start_section_len = wasm_ui32.value;
                index = index + wasm_ui32.value + wasm_ui32.len;
                break;
            }
            case 9:{
                // Element Section
                bin_mod.element_section = index - 1;
                wasm_ui32 = DecodeLeb128UInt32(index);
                bin_mod.element_section_len = wasm_ui32.value;
                index = index + wasm_ui32.value + wasm_ui32.len;
                break;
            }
            case 10:{
                //Code Section   
                bin_mod.code_section = index - 1;
                wasm_ui32 = DecodeLeb128UInt32(index);
                bin_mod.code_section_len = wasm_ui32.value;
                index = index + wasm_ui32.value + wasm_ui32.len;
                break;
            }
            case 11:{
                //Data Section   
                bin_mod.data_section = index - 1;
                wasm_ui32 = DecodeLeb128UInt32(index);
                bin_mod.data_section_len = wasm_ui32.value;
                index = index + wasm_ui32.value + wasm_ui32.len;
                break;
            }
            case 12:{
                //Data Section   
                bin_mod.data_count_section = index - 1;
                wasm_ui32 = DecodeLeb128UInt32(index);
                bin_mod.data_count_section_len = wasm_ui32.value;
                index = index + wasm_ui32.value + wasm_ui32.len;
                break;
            }
            default:{
                //Not a valid Section ID
                bin_mod = NULL_BINARY_MODULE;
                bin_mod.load_status = ERR_SECTION_ID;     
                return bin_mod;           
            }

        }
        //increment counter for avoid endless loop
        section_counter ++;
    }
    
    #undef READ_BYTE
    #undef NOT_END

    return bin_mod;
}/*

/**
 * @brief Function to validate wasm binary magic number
 * 
 * @param wasm wasm binary format
 * @param len len of wasm magic byte array, must be equal to 4.
 * @return LoaderResult 
 *
LoaderResult ValidateMagic(uint8_t *wasm, size_t len){
    //Fail if is called with invalid len parameter
    assert(len == 4);
    uint8_t magic_number_bytes[4] = {0x00, 0x61, 0x73, 0x6D};       /// Magic number that all wasm file start
    uint32_t wasm_magic_number = *((uint32_t *)magic_number_bytes);      /// magic number in uint32 format to avoid endianess problem
    uint32_t file_magic_number;

    // Magic number check
    file_magic_number = *((uint32_t *)wasm);    
    if(file_magic_number != wasm_magic_number){
        return ERR_MAGIC;
    }
    else return LOAD_OK;
}*/

/**
 * @brief Function to validate wasm binary version number
 * 
 * @param wasm wasm binary format
 * @param len len of wasm version byte array, must be equal to 4.
 * @return LoaderResult 
 *
LoaderResult ValidateVersion(uint8_t *wasm, size_t len){
    //Fail if is called with invalid len parameter
    assert(len == 4);
    uint8_t version_number_bytes[4] = {0x01, 0x00, 0x00, 0x00};     /// version number    
    uint32_t version_number_1 = *((uint32_t *)version_number_bytes);  /// version number in uint32 format to avoid endianess problem
    uint32_t file_version_number;

    file_version_number = *((uint32_t *)wasm);    
    if(file_version_number != version_number_1){
        return ERR_VERSION;
    }
    else return LOAD_OK;
}*/



