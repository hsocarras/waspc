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
#include "memory/memory.h"

#include <string.h>
#include <stdio.h> //TODO remove

/**
 * @brief Function to validate wasm binary magic number
 * 
 * @param buf wasm binary format 
 * @return WpError 
 */
static WpError ValidateMagic(const uint8_t *buf){

    WpError result = CreateError(OK);       //No error
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
    
    WpError result = CreateError(OK);       //No error

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

static void InitWasmBin(WasmBinModule * module){

    module->version = 0;

    module->typesec.size = 0;
    module->typesec.content = NULL;

    module->importsec.size = 0;
    module->importsec.content = NULL;

    module->functionsec.size = 0;
    module->functionsec.content = NULL;

    module->tablesec.size = 0;
    module->tablesec.content = NULL;

    module->memsec.size = 0;
    module->memsec.content = NULL;

    module->globalsec.size = 0;
    module->globalsec.content = NULL;

    module->exportsec.size = 0;
    module->exportsec.content = NULL;

    module->startsec.size = 0;
    module->startsec.content = NULL;

    module->elemsec.size = 0;
    module->elemsec.content = NULL;

    module->datacountsec.size = 0;
    module->datacountsec.content = NULL;

    module->codesec.size = 0;
    module->codesec.content = NULL;

    module->datasec.size = 0;
    module->datasec.content = NULL;
}

/**
 * @brief 
 * 
 * @param module 
 * @param buf 
 * @param size 
 * @return WpError 
 */
static WpError LoadWasmBin(const uint8_t *buf, uint32_t size, WasmBinModule *module) {

    WpError result = CreateError(OK);       //No error

    const uint8_t *index = buf;                         // pointer to byte to traverse the binary file
    const uint8_t *buf_end = buf + size;                // pointer to end of binary module
    uint8_t section_counter = 0;                        // auxiliary variable for avoid endless loop
    DEC_UINT32_LEB128 wasm_u32;                         // auxiliary variable to decode leb128 values
    uint32_t binary_version;
    uint8_t section_id;
    uint32_t section_size;  
    
    WasmBinSection section = {0, NULL};

    
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
                //custom section startwill be ignored 
                wasm_u32 = DecodeLeb128UInt32(index);
                if (wasm_u32.len != 0){                    
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

                if(module->typesec.content != NULL){
                    //Check that type section is in init state.
                    //Otherwise significa que esta repetida en el archivo
                    result.id = INVALID_SECTION_ID;     //TODO better error for duplicate section
                    return result;
                }
                
                wasm_u32 = DecodeLeb128UInt32(index);
                if (wasm_u32.len == 0){
                    result.id = INVALID_SECTION_ID;     //TODO better error for wrong leb128 encoded value
                    return result;                                        
                }

                section.size = wasm_u32.value;
                section.content = index + wasm_u32.len;
                index = section.content + wasm_u32.value;

                module->typesec.size = section.size;
                module->typesec.content = section.content;
                break;
            }
            case IMPORT:{
                // import Section                
                if(module->importsec.content != NULL){
                    //Check that custom section is in init state.
                    //Otherwise significa que esta repetida en el archivo
                    result.id = INVALID_SECTION_ID;     //TODO better error for duplicate section
                    return result;
                }
                
                wasm_u32 = DecodeLeb128UInt32(index);
                if (wasm_u32.len == 0){
                    result.id = INVALID_SECTION_ID;     //TODO better error for wrong leb128 encoded value
                    return result;                                        
                }

                section.size = wasm_u32.value;
                section.content = index + wasm_u32.len;
                index = section.content + wasm_u32.value;

                module->importsec.size = section.size;
                module->importsec.content = section.content;
                break;
            }
            case FUNCTION:{
                // Function Section
                if(module->functionsec.content != NULL){
                    //Check that custom section is in init state.
                    //Otherwise significa que esta repetida en el archivo
                    result.id = INVALID_SECTION_ID;     //TODO better error for duplicate section
                    return result;
                }
                
                wasm_u32 = DecodeLeb128UInt32(index);
                if (wasm_u32.len == 0){
                    result.id = INVALID_SECTION_ID;     //TODO better error for wrong leb128 encoded value
                    return result;                                        
                }

                section.size = wasm_u32.value;
                section.content = index + wasm_u32.len;
                index = section.content + wasm_u32.value;

                module->functionsec.size = section.size;
                module->functionsec.content = section.content;
                break;
            }
            case TABLE:{
                // Table Section
                if(module->tablesec.content != NULL){
                    //Check that custom section is in init state.
                    //Otherwise significa que esta repetida en el archivo
                    result.id = INVALID_SECTION_ID;     //TODO better error for duplicate section
                    return result;
                }
                
                wasm_u32 = DecodeLeb128UInt32(index);
                if (wasm_u32.len == 0){
                    result.id = INVALID_SECTION_ID;     //TODO better error for wrong leb128 encoded value
                    return result;                                        
                }

                section.size = wasm_u32.value;
                section.content = index + wasm_u32.len;
                index = section.content + wasm_u32.value;

                module->tablesec.size = section.size;
                module->tablesec.content = section.content;
                break;
            }
            case MEMORY:{
                // Memory Section
                if(module->memsec.content != NULL){
                    //Check that custom section is in init state.
                    //Otherwise significa que esta repetida en el archivo
                    result.id = INVALID_SECTION_ID;     //TODO better error for duplicate section
                    return result;
                }
                
                wasm_u32 = DecodeLeb128UInt32(index);
                if (wasm_u32.len == 0){
                    result.id = INVALID_SECTION_ID;     //TODO better error for wrong leb128 encoded value
                    return result;                                        
                }

                section.size = wasm_u32.value;
                section.content = index + wasm_u32.len;
                index = section.content + wasm_u32.value;

                module->memsec.size = section.size;
                module->memsec.content = section.content;
                break;
            }
            case GLOBAL:{
                // GLOBAL Section
                if(module->globalsec.content != NULL){
                    //Check that custom section is in init state.
                    //Otherwise significa que esta repetida en el archivo
                    result.id = INVALID_SECTION_ID;     //TODO better error for duplicate section
                    return result;
                }
                
                wasm_u32 = DecodeLeb128UInt32(index);
                if (wasm_u32.len == 0){
                    result.id = INVALID_SECTION_ID;     //TODO better error for wrong leb128 encoded value
                    return result;                                        
                }

                section.size = wasm_u32.value;
                section.content = index + wasm_u32.len;
                index = section.content + wasm_u32.value;

                module->globalsec.size = section.size;
                module->globalsec.content = section.content;
                break;
            }
            case EXPORT:{
                //Export Section
                if(module->exportsec.content != NULL){
                    //Check that custom section is in init state.
                    //Otherwise significa que esta repetida en el archivo
                    result.id = INVALID_SECTION_ID;     //TODO better error for duplicate section
                    return result;
                }
                
                wasm_u32 = DecodeLeb128UInt32(index);
                if (wasm_u32.len == 0){
                    result.id = INVALID_SECTION_ID;     //TODO better error for wrong leb128 encoded value
                    return result;                                        
                }

                section.size = wasm_u32.value;
                section.content = index + wasm_u32.len;
                index = section.content + wasm_u32.value;

                module->exportsec.size = section.size;
                module->exportsec.content = section.content;
                break;
            }
            case START:{
                // Start Section
                if(module->startsec.content != NULL){
                    //Check that custom section is in init state.
                    //Otherwise significa que esta repetida en el archivo
                    result.id = INVALID_SECTION_ID;     //TODO better error for duplicate section
                    return result;
                }
                
                wasm_u32 = DecodeLeb128UInt32(index);
                if (wasm_u32.len == 0){
                    result.id = INVALID_SECTION_ID;     //TODO better error for wrong leb128 encoded value
                    return result;                                        
                }

                section.size = wasm_u32.value;
                section.content = index + wasm_u32.len;
                index = section.content + wasm_u32.value;

                module->startsec.size = section.size;
                module->startsec.content = section.content;
                break;
            }
            case ELEMENT:{
                // Element Section
                if(module->elemsec.content != NULL){
                    //Check that custom section is in init state.
                    //Otherwise significa que esta repetida en el archivo
                    result.id = INVALID_SECTION_ID;     //TODO better error for duplicate section
                    return result;
                }
                
                wasm_u32 = DecodeLeb128UInt32(index);
                if (wasm_u32.len == 0){
                    result.id = INVALID_SECTION_ID;     //TODO better error for wrong leb128 encoded value
                    return result;                                        
                }

                section.size = wasm_u32.value;
                section.content = index + wasm_u32.len;
                index = section.content + wasm_u32.value;

                module->elemsec.size = section.size;
                module->elemsec.content = section.content;
                break;
            }
            case CODE:{
                //Code Section   
                if(module->codesec.content != 0){
                    //Check that custom section is in init state.
                    //Otherwise significa que esta repetida en el archivo
                    result.id = INVALID_SECTION_ID;     //TODO better error for duplicate section
                    return result;
                }
                
                wasm_u32 = DecodeLeb128UInt32(index);
                if (wasm_u32.len == 0){
                    result.id = INVALID_SECTION_ID;     //TODO better error for wrong leb128 encoded value
                    return result;                                        
                }

                section.size = wasm_u32.value;
                section.content = index + wasm_u32.len;
                index = section.content + wasm_u32.value;

                module->codesec.size = section.size;
                module->codesec.content = section.content;
                break;
            }
            case DATA:{
                //Data Section   
                if(module->datasec.content != NULL){
                    //Check that custom section is in init state.
                    //Otherwise significa que esta repetida en el archivo
                    result.id = INVALID_SECTION_ID;     //TODO better error for duplicate section
                    return result;
                }
                
                wasm_u32 = DecodeLeb128UInt32(index);
                if (wasm_u32.len == 0){
                    result.id = INVALID_SECTION_ID;     //TODO better error for wrong leb128 encoded value
                    return result;                                        
                }

                section.size = wasm_u32.value;
                section.content = index + wasm_u32.len;
                index = section.content + wasm_u32.value;

                module->datasec.size = section.size;
                module->datasec.content = section.content;
                break;
            }
            case DATA_COUNT:{
                //Data Count Section   
                if(module->datacountsec.content != NULL){
                    //Check that custom section is in init state.
                    //Otherwise significa que esta repetida en el archivo
                    result.id = INVALID_SECTION_ID;     //TODO better error for duplicate section
                    return result;
                }
                
                wasm_u32 = DecodeLeb128UInt32(index);
                if (wasm_u32.len == 0){
                    result.id = INVALID_SECTION_ID;     //TODO better error for wrong leb128 encoded value
                    return result;                                        
                }

                section.size = wasm_u32.value;
                section.content = index + wasm_u32.len;
                index = section.content + wasm_u32.value;

                module->datacountsec.size = section.size;
                module->datacountsec.content = section.content;
                break;
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
 * @brief function that get a binary wasm buffer from persistent storage and stored in ram's
 * load memory area.
 * 
 * @param self. Runtime enviroment.
 * @param buf A buffer with module content.
 * @param size  buffer size.
 * @param id Hash for pou's name.
 * @return WpError 
 */
WpError LoadWasmBuffer(RuntimeEnv *self, const uint8_t *buf, uint32_t size, uint32_t id) {

    WpError result = CreateError(OK);       //No error
    
    WasmBinModule *mod = ALLOCATE(WasmBinModule, 1);
    InitWasmBin(mod);

    //TODO load buffer into code memory. righ now comes from driver
    mod->bin_len = size;
    mod->start = self->code_mem;
    result = LoadWasmBin(self->code_mem, size, mod);
    
    if(result.id != OK){
        //Clean process TODO
        return result;
    }

    result = HashTableSet(&self->table_wasm_bin, id, (void *)mod);
    //self->load_mem_manager.block_count++;

    return result;

    
}