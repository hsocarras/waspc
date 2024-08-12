/**
 * @file wasm_loader.c
 * @author Hector E. Socarras (hsocarras1987@gmail.com)
 * @brief Header file for wasm load module
 * @version 0.1
 * @date 2024-06-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#include "loader/wasm_loader.h"
#include "diagnostic/error.h"
#include "webassembly/binary/module.h"
#include "utils/leb128.h"

/**
 * @brief Function to validate wasm binary magic number
 * 
 * @param buf wasm binary format 
 * @return WpError 
 */
static WpError ValidateMagic(const uint8_t *buf){

    WpError result = {OK};       //No error
    uint8_t magic_number_bytes[4] = {0x00, 0x61, 0x73, 0x6D};           /// Magic number that all wasm file start
    uint32_t wasm_magic_number = *((uint32_t *)magic_number_bytes);     /// magic number in uint32 format to avoid endianess problem
    uint32_t file_magic_number;

    // Magic number check
    file_magic_number = *((uint32_t *)buf);    
    if(file_magic_number != wasm_magic_number){
        result.id = INVALID_MAGIC;
        return result;
    }
    else return result;
}

/**
 * @brief Function to validate wasm binary version number
 * 
 * @param buf wasm binary format 
 * @return WpError 
 */
static WpError ValidateVersion(const uint8_t *buf, uint32_t *version_number){
    
    WpError result = {OK};       //No error

    uint8_t version_number_bytes[4] = {0x01, 0x00, 0x00, 0x00};         /// version number    
    uint32_t version_number_1 = *((uint32_t *)version_number_bytes);    /// version number in uint32 format to avoid endianess problem
    uint32_t file_version_number;

    file_version_number = *((uint32_t *)buf); 

    if(file_version_number != version_number_1){
        result.id = INVALID_VERSION;
        return result;
    }
    else {

        *version_number = 1;
        return result;
    } 
}

/**
 * @brief Create a Module object
 * 
 * @param self 
 * @return WpError 
 */
static WpError CreateModule(Vm *self) {

    WpError result = {OK};       //No error
    
    //TODO try allocate module into VM Load Memory
    //for now no alloc is implemented

    return result;

}

static WpError load(Vm *self, const uint8_t *buf, uint32_t size) {

    WpError result = {OK};            //No error

    const uint8_t *index = buf;                         // pointer to byte to traverse the binary file
    const uint8_t *buf_end = buf + size;                // pointer to end of binary module
    uint8_t section_counter = 0;                        // auxiliary variable for avoid endless loop
    DEC_UINT32_LEB128 wasm_u32;                         // auxiliary variable to decode leb128 values
    uint32_t binary_version;
    uint8_t section_id;
    uint32_t section_size;  

    //// CAN CHANGE LATER ////////////////////////////////////////////////////////////////////////                                
    // Array for store the sections in the binary file
    // If not necesary store in the WasmModule struct where the sections start
    // Will keep this implementation to save memory in the WasmModule struct
    // otherwhise the WasmModule struct will have the twelve section struct
    WasmModuleSection sections[13] = {{0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
                                        {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};     

    
    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    // Check minimun buffer length for magic and version number ///////////////////////////////////
    if(size < 8){
        //if buffer's size is less than 4 is a wrong binary module
        result.id = INVALID_MODULE_SIZE;
        return result;
    }

    // Check magic number /////////////////////////////////////////////////////////////////////////
    result = ValidateMagic(index);
    if (result.id != OK){
        return result;
    }   
    index = index + 4; 
    ///////////////////////////////////////////////////////////////////////////////////////////////

    // Check version number /////////////////////////////////////////////////////////////////////////
    result = ValidateVersion(index, &binary_version);
    if (result.id != OK){
        return result;
    }
    index = index + 4;
    self->main_module.package_version = binary_version;
    ///////////////////////////////////////////////////////////////////////////////////////////////

    //LOOP to traverse the binary file
    while(NOT_END() && section_counter <= DATA_COUNT){
        //Seccion
        section_id = READ_BYTE();
        
        switch(section_id){
            case CUSTOM:{
                //custom section start
                if(sections[0].section_size != 0){
                    //Check that custom section is in init state.
                    //Otherwise significa que esta repetida en el archivo
                    result.id = INVALID_SECTION_ID;     //TODO better error for duplicate section
                }
                sections[0].section_content = index - 1;
                wasm_u32 = DecodeLeb128UInt32(index);
                if (wasm_u32.len != 0){
                    sections[0].section_size = wasm_u32.value;
                    index = index + wasm_u32.len + wasm_u32.value;
                    break;
                }
                else {
                    result.id = INVALID_SECTION_ID;     //TODO better error for wrong leb128 encoded value
                    return result;
                }                
            }
            case TYPE:{
                // Type Section
                if(sections[1].section_size != 0){
                    //Check that custom section is in init state.
                    //Otherwise significa que esta repetida en el archivo
                    result.id = INVALID_SECTION_ID;     //TODO better error for duplicate section
                }
                sections[1].section_content = index - 1;
                wasm_u32 = DecodeLeb128UInt32(index);
                if (wasm_u32.len != 0){
                    sections[1].section_size = wasm_u32.value;
                    index = index + wasm_u32.len + wasm_u32.value;
                    break;
                }
                else {
                    result.id = INVALID_SECTION_ID;     //TODO better error for wrong leb128 encoded value
                    return result;
                }
            }
            case IMPORT:{
                // import Section                
                if(sections[2].section_size != 0){
                    //Check that custom section is in init state.
                    //Otherwise significa que esta repetida en el archivo
                    result.id = INVALID_SECTION_ID;     //TODO better error for duplicate section
                }
                sections[2].section_content = index - 1;
                wasm_u32 = DecodeLeb128UInt32(index);
                if (wasm_u32.len != 0){
                    sections[2].section_size = wasm_u32.value;
                    index = index + wasm_u32.len + wasm_u32.value;
                    break;
                }
                else {
                    result.id = INVALID_SECTION_ID;     //TODO better error for wrong leb128 encoded value
                    return result;
                }
            }
            case FUNCTION:{
                // Function Section
                if(sections[3].section_size != 0){
                    //Check that custom section is in init state.
                    //Otherwise significa que esta repetida en el archivo
                    result.id = INVALID_SECTION_ID;     //TODO better error for duplicate section
                }
                sections[3].section_content = index - 1;
                wasm_u32 = DecodeLeb128UInt32(index);
                if (wasm_u32.len != 0){
                    sections[3].section_size = wasm_u32.value;
                    index = index + wasm_u32.len + wasm_u32.value;
                    break;
                }
                else {
                    result.id = INVALID_SECTION_ID;     //TODO better error for wrong leb128 encoded value
                    return result;
                }
            }
            case TABLE:{
                // Table Section
                if(sections[4].section_size != 0){
                    //Check that custom section is in init state.
                    //Otherwise significa que esta repetida en el archivo
                    result.id = INVALID_SECTION_ID;     //TODO better error for duplicate section
                }
                sections[4].section_content = index - 1;
                wasm_u32 = DecodeLeb128UInt32(index);
                if (wasm_u32.len != 0){
                    sections[4].section_size = wasm_u32.value;
                    index = index + wasm_u32.len + wasm_u32.value;
                    break;
                }
                else {
                    result.id = INVALID_SECTION_ID;     //TODO better error for wrong leb128 encoded value
                    return result;
                }
            }
            case MEMORY:{
                // Memory Section
                if(sections[5].section_size != 0){
                    //Check that custom section is in init state.
                    //Otherwise significa que esta repetida en el archivo
                    result.id = INVALID_SECTION_ID;     //TODO better error for duplicate section
                }
                sections[5].section_content = index - 1;
                wasm_u32 = DecodeLeb128UInt32(index);
                if (wasm_u32.len != 0){
                    sections[5].section_size = wasm_u32.value;
                    index = index + wasm_u32.len + wasm_u32.value;
                    break;
                }
                else {
                    result.id = INVALID_SECTION_ID;     //TODO better error for wrong leb128 encoded value
                    return result;
                }
            }
            case GLOBAL:{
                // GLOBAL Section
                if(sections[6].section_size != 0){
                    //Check that custom section is in init state.
                    //Otherwise significa que esta repetida en el archivo
                    result.id = INVALID_SECTION_ID;     //TODO better error for duplicate section
                }
                sections[6].section_content = index - 1;
                wasm_u32 = DecodeLeb128UInt32(index);
                if (wasm_u32.len != 0){
                    sections[6].section_size = wasm_u32.value;
                    index = index + wasm_u32.len + wasm_u32.value;
                    break;
                }
                else {
                    result.id = INVALID_SECTION_ID;     //TODO better error for wrong leb128 encoded value
                    return result;
                }
            }
            case EXPORT:{
                //Export Section
                if(sections[7].section_size != 0){
                    //Check that custom section is in init state.
                    //Otherwise significa que esta repetida en el archivo
                    result.id = INVALID_SECTION_ID;     //TODO better error for duplicate section
                }
                sections[7].section_content = index - 1;
                wasm_u32 = DecodeLeb128UInt32(index);
                if (wasm_u32.len != 0){
                    sections[7].section_size = wasm_u32.value;
                    index = index + wasm_u32.len + wasm_u32.value;
                    break;
                }
                else {
                    result.id = INVALID_SECTION_ID;     //TODO better error for wrong leb128 encoded value
                    return result;
                }
            }
            case START:{
                // Start Section
                if(sections[8].section_size != 0){
                    //Check that custom section is in init state.
                    //Otherwise significa que esta repetida en el archivo
                    result.id = INVALID_SECTION_ID;     //TODO better error for duplicate section
                }
                sections[8].section_content = index - 1;
                wasm_u32 = DecodeLeb128UInt32(index);
                if (wasm_u32.len != 0){
                    sections[8].section_size = wasm_u32.value;
                    index = index + wasm_u32.len + wasm_u32.value;
                    break;
                }
                else {
                    result.id = INVALID_SECTION_ID;     //TODO better error for wrong leb128 encoded value
                    return result;
                }
            }
            case ELEMENT:{
                // Element Section
                if(sections[9].section_size != 0){
                    //Check that custom section is in init state.
                    //Otherwise significa que esta repetida en el archivo
                    result.id = INVALID_SECTION_ID;     //TODO better error for duplicate section
                }
                sections[9].section_content = index - 1;
                wasm_u32 = DecodeLeb128UInt32(index);
                if (wasm_u32.len != 0){
                    sections[9].section_size = wasm_u32.value;
                    index = index + wasm_u32.len + wasm_u32.value;
                    break;
                }
                else {
                    result.id = INVALID_SECTION_ID;     //TODO better error for wrong leb128 encoded value
                    return result;
                }
            }
            case CODE:{
                //Code Section   
                if(sections[10].section_size != 0){
                    //Check that custom section is in init state.
                    //Otherwise significa que esta repetida en el archivo
                    result.id = INVALID_SECTION_ID;     //TODO better error for duplicate section
                }
                sections[10].section_content = index - 1;
                wasm_u32 = DecodeLeb128UInt32(index);
                if (wasm_u32.len != 0){
                    sections[10].section_size = wasm_u32.value;
                    index = index + wasm_u32.len + wasm_u32.value;
                    break;
                }
                else {
                    result.id = INVALID_SECTION_ID;     //TODO better error for wrong leb128 encoded value
                    return result;
                }
            }
            case DATA:{
                //Data Section   
                if(sections[11].section_size != 0){
                    //Check that custom section is in init state.
                    //Otherwise significa que esta repetida en el archivo
                    result.id = INVALID_SECTION_ID;     //TODO better error for duplicate section
                }
                sections[11].section_content = index - 1;
                wasm_u32 = DecodeLeb128UInt32(index);
                if (wasm_u32.len != 0){
                    sections[11].section_size = wasm_u32.value;
                    index = index + wasm_u32.len + wasm_u32.value;
                    break;
                }
                else {
                    result.id = INVALID_SECTION_ID;     //TODO better error for wrong leb128 encoded value
                    return result;
                }
            }
            case DATA_COUNT:{
                //Data Count Section   
                if(sections[12].section_size != 0){
                    //Check that custom section is in init state.
                    //Otherwise significa que esta repetida en el archivo
                    result.id = INVALID_SECTION_ID;     //TODO better error for duplicate section
                }
                sections[12].section_content = index - 1;
                wasm_u32 = DecodeLeb128UInt32(index);
                if (wasm_u32.len != 0){
                    sections[12].section_size = wasm_u32.value;
                    index = index + wasm_u32.len + wasm_u32.value;
                    break;
                }
                else {
                    result.id = INVALID_SECTION_ID;     //TODO better error for wrong leb128 encoded value
                    return result;
                }
            }
            default:{
                //Not a valid Section ID
                result.id = INVALID_SECTION_ID;     //TODO better error for duplicate section
                return result;           
            }

        }
        //increment counter for avoid endless loop
        section_counter ++;
    }
    
    return result;

    #undef READ_BYTE
    #undef NOT_END
}

/**
 * @brief 
 * 
 * @param self 
 * @param buf 
 * @param size 
 * @param error_buf 
 * @param error_buf_size 
 * @return WpError 
 */
WpError LoadWasm(Vm *self, const uint8_t *buf, uint32_t size) {

    WpError result = {OK};       //No error

    // Allocating new module into the runtime
    result = CreateModule(self);

    if (result.id != 0) {
        return result;
    }


    result = load(self, buf, size);

    return result;


    
}