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

#include "runtime/runtime.h"
#include "loader/wasm_loader.h"
#include "diagnostic/error.h"
#include "webassembly/binary/module.h"
#include "utils/leb128.h"

#include <string.h>

/**
 * @brief Function to validate wasm binary magic number
 * 
 * @param buf wasm binary format 
 * @return WpError 
 *
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
}*/

/**
 * @brief Function to validate wasm binary version number
 * 
 * @param buf wasm binary format 
 * @return WpError 
 *
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
}*/

/**
 * @brief Create a Module object
 * 
 * @param 
 * @return WpError 
 *
static WpError CreateModule() {

    WpError result = {OK};       //No error
    
    //TODO try allocate module into VM Load Memory
    //for now no alloc is implemented

    return result;

}*/
/*
static WpError LoadCustomSection(WasmModule *mod, WasmModuleSection custom_section){
    WpError result = {OK};            //No error
    return result;
}

static WpError LoadTypeSection(WasmModule *mod, WasmModuleSection type_section){
    WpError result = {OK};            //No error
    const uint8_t *index = type_section.section_content;                        // pointer to byte to traverse the binary file
    const uint8_t *buf_end = index + type_section.section_size;                 // pointer to end of binary module
    DEC_UINT32_LEB128 wasm_u32;                                                 // auxiliary variable to decode leb128 values
    uint32_t type_count;

    wasm_u32 = DecodeLeb128UInt32(index);
    if (wasm_u32.len != 0){
        type_count = wasm_u32.value;
        mod->type_count = type_count;
        index = index + wasm_u32.len;   
    }
    else {
        result.id = INVALID_SECTION_ID;     //TODO better error for wrong leb128 encoded value
        return result;
    }
    return result;
}

static WpError LoadSections (WasmModule *mod, WasmModuleSection * sections){

    WpError result = {OK};            //No error

    if(sections[0].section_size > 0){
        result = LoadCustomSection(mod, sections[0]);
        if (result.id != 0){
            return result;
        }
    }

    if(sections[1].section_size > 0){
        result = LoadTypeSection(mod, sections[0]);
        if (result.id != 0){
            return result;
        }
    }

    return result;

}*/

/**
 * @brief 
 * 
 * @param module 
 * @param buf 
 * @param size 
 * @return WpError 
 *
static WpError load(WasmModule *module, const uint8_t *buf, uint32_t size) {

    WpError result = {OK};            //No error

    const uint8_t *index = buf;                         // pointer to byte to traverse the binary file
    const uint8_t *buf_end = buf + size;                // pointer to end of binary module
    uint8_t section_counter = 0;                        // auxiliary variable for avoid endless loop
    DEC_UINT32_LEB128 wasm_u32;                         // auxiliary variable to decode leb128 values
    uint32_t binary_version;
    uint8_t section_id;
    uint32_t section_size;  

    //// Inicialise module//////////////////////////////////////////////////////////////////////// 
    *module = {0, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0},
                {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}, {0, 0}};     

    
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
    module->version = binary_version;
    ///////////////////////////////////////////////////////////////////////////////////////////////

    //LOOP to traverse the binary file
    while(NOT_END() && section_counter <= DATA_COUNT){
        //Seccion
        section_id = READ_BYTE();
        
        switch(section_id){
            case CUSTOM:{
                //custom section start
                if(module->custom.section_size != 0){
                    //Check that custom section is in init state.
                    //Otherwise significa que esta repetida en el archivo
                    result.id = INVALID_SECTION_ID;     //TODO better error for duplicate section
                }
                module->custom.section_content = index - 1;
                wasm_u32 = DecodeLeb128UInt32(index);
                if (wasm_u32.len != 0){
                    module->custom.section_size = wasm_u32.value;
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
                if(module->type.section_size != 0){
                    //Check that custom section is in init state.
                    //Otherwise significa que esta repetida en el archivo
                    result.id = INVALID_SECTION_ID;     //TODO better error for duplicate section
                }
                module->type.section_content = index - 1;
                wasm_u32 = DecodeLeb128UInt32(index);
                if (wasm_u32.len != 0){
                    module->type.section_size = wasm_u32.value;
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
                if(module->import.section_size != 0){
                    //Check that custom section is in init state.
                    //Otherwise significa que esta repetida en el archivo
                    result.id = INVALID_SECTION_ID;     //TODO better error for duplicate section
                }
                module->import.section_content = index - 1;
                wasm_u32 = DecodeLeb128UInt32(index);
                if (wasm_u32.len != 0){
                    module->import.section_size = wasm_u32.value;
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
                if(module->function.section_size != 0){
                    //Check that custom section is in init state.
                    //Otherwise significa que esta repetida en el archivo
                    result.id = INVALID_SECTION_ID;     //TODO better error for duplicate section
                }
                module->function.section_content = index - 1;
                wasm_u32 = DecodeLeb128UInt32(index);
                if (wasm_u32.len != 0){
                    module->function.section_size = wasm_u32.value;
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
                if(module->table.section_size != 0){
                    //Check that custom section is in init state.
                    //Otherwise significa que esta repetida en el archivo
                    result.id = INVALID_SECTION_ID;     //TODO better error for duplicate section
                }
                module->table.section_content = index - 1;
                wasm_u32 = DecodeLeb128UInt32(index);
                if (wasm_u32.len != 0){
                    module->table.section_size = wasm_u32.value;
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
                if(module->memory.section_size != 0){
                    //Check that custom section is in init state.
                    //Otherwise significa que esta repetida en el archivo
                    result.id = INVALID_SECTION_ID;     //TODO better error for duplicate section
                }
                module->memory.section_content = index - 1;
                wasm_u32 = DecodeLeb128UInt32(index);
                if (wasm_u32.len != 0){
                    module->memory.section_size = wasm_u32.value;
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
                if(module->global.section_size != 0){
                    //Check that custom section is in init state.
                    //Otherwise significa que esta repetida en el archivo
                    result.id = INVALID_SECTION_ID;     //TODO better error for duplicate section
                }
                module->global.section_content = index - 1;
                wasm_u32 = DecodeLeb128UInt32(index);
                if (wasm_u32.len != 0){
                    module->global.section_size = wasm_u32.value;
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
                if(module->export.section_size != 0){
                    //Check that custom section is in init state.
                    //Otherwise significa que esta repetida en el archivo
                    result.id = INVALID_SECTION_ID;     //TODO better error for duplicate section
                }
                module->export.section_content = index - 1;
                wasm_u32 = DecodeLeb128UInt32(index);
                if (wasm_u32.len != 0){
                    module->export.section_size = wasm_u32.value;
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
                if(module->start.section_size != 0){
                    //Check that custom section is in init state.
                    //Otherwise significa que esta repetida en el archivo
                    result.id = INVALID_SECTION_ID;     //TODO better error for duplicate section
                }
                module->start.section_content = index - 1;
                wasm_u32 = DecodeLeb128UInt32(index);
                if (wasm_u32.len != 0){
                    module->start.section_size = wasm_u32.value;
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
                if(module->element.section_size != 0){
                    //Check that custom section is in init state.
                    //Otherwise significa que esta repetida en el archivo
                    result.id = INVALID_SECTION_ID;     //TODO better error for duplicate section
                }
                module->element.section_content = index - 1;
                wasm_u32 = DecodeLeb128UInt32(index);
                if (wasm_u32.len != 0){
                    module->element.section_size = wasm_u32.value;
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
                if(module->code.section_size != 0){
                    //Check that custom section is in init state.
                    //Otherwise significa que esta repetida en el archivo
                    result.id = INVALID_SECTION_ID;     //TODO better error for duplicate section
                }
                module->code.section_content = index - 1;
                wasm_u32 = DecodeLeb128UInt32(index);
                if (wasm_u32.len != 0){
                    module->code.section_size = wasm_u32.value;
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
                if(module->data.section_size != 0){
                    //Check that custom section is in init state.
                    //Otherwise significa que esta repetida en el archivo
                    result.id = INVALID_SECTION_ID;     //TODO better error for duplicate section
                }
                module->data.section_content = index - 1;
                wasm_u32 = DecodeLeb128UInt32(index);
                if (wasm_u32.len != 0){
                    module->data.section_size = wasm_u32.value;
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
                if(module->data_count.section_size != 0){
                    //Check that custom section is in init state.
                    //Otherwise significa que esta repetida en el archivo
                    result.id = INVALID_SECTION_ID;     //TODO better error for duplicate section
                }
                module->data_count.section_content = index - 1;
                wasm_u32 = DecodeLeb128UInt32(index);
                if (wasm_u32.len != 0){
                    module->data_count.section_size = wasm_u32.value;
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
}*/

/**
 * @brief function that get a binary wasm buffer from persistent storage and stored in ram's
 * load memory area.
 * 
 * @param self. Runtime enviroment.
 * @param buf A buffer with module content.
 * @param size  buffer size.
 * @param buf_name Original pou's name.
 * @return WpError 
 */
WpError LoadWasmBuffer(RuntimeEnv *self, const uint8_t *buf, uint32_t size, const char *buf_name) {

    WpError result = CreateError(OK);       //No error

    //Check if are load memory available
    uint32_t free_memory = GetFreeLoadMemory(&self->load_mem_manager);
    uint32_t len;
    if(free_memory < size){
        result.id = 3;              //TODO
    }

    //TODO check name's lenght
    uint8_t *current_load_pos = self->load_mem_manager.index;
    result = AppendBufferToLoadMem(&self->load_mem_manager, buf, size);

    if(result.id != OK){
        //Clean process TODO
    }
   
    //self->block_loaded[self->load_mem_manager.block_index].name = buf_name;
    len = strlen(buf_name);
    strcpy_s(self->block_loaded[self->load_mem_manager.block_count].name, len+1, buf_name);
    self->block_loaded[self->load_mem_manager.block_count].size = size;
    self->block_loaded[self->load_mem_manager.block_count].start = current_load_pos;

    self->load_mem_manager.block_count++;

    return result;


    
}