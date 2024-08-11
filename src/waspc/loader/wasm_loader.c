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

    const uint8_t *index = buf;                       // pointer to byte to traverse the binary file
    //const uint8 *p = buf, *p_end = buf_end;

    uint32_t binary_version;
    //WASMSection *section_list = NULL;

    #define READ_BYTE() (*index++)

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
    /*

    if (!create_sections(buf, size, &section_list, error_buf, error_buf_size)
        || !load_from_sections(module, section_list, true, wasm_binary_freeable,
                               error_buf, error_buf_size)) {
        destroy_sections(section_list);
        return false;
    }

    destroy_sections(section_list);
    return true;
fail:*/
    return result;

    #undef READ_BYTE
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