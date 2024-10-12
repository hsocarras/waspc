/**
 * @file wasm_loader.c
 * @author Hector E. Socarras (hsocarras1987@gmail.com)
 * @brief Header file for wasm load module. The purpose for this module is create the struct WasmBinModule
 * in runtime hash_table for bin. Make some basic validation like magic number and version and invoke the decoder module.
 * @version 0.1
 * @date 2024-06-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#include "loader/wasm_loader.h"
#include "utils/leb128.h"
#include "memory/memory.h"

#include <string.h>


/**
 * @brief Function to validate wasm binary magic number
 *      magic ::= 0x00 0x61 0x73 0x6D
 * @param buf wasm binary format 
 * @return uint8_t 1 - ok, 0 - invalid 
 */
static uint8_t ValidateMagic(const uint8_t *buf){

   
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
static uint8_t ValidateVersion(const uint8_t *buf, uint32_t *version_number){
    
    

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

static const uint8_t * LoadBinSection(const uint8_t *index, WasmBinSection *sec){

    uint32_t aux_u32;
    index = DecodeLeb128UInt32(index, &aux_u32);
    if (!index){
        return NULL;                                       
    }

    sec->size = aux_u32;
    sec->content = index;
    index = index + aux_u32;

    return index;

}

static const uint8_t* LoadBinSectionById(const uint8_t *index, const uint8_t section_id, uint8_t *last_loaded_sec, WasmBinModule *bin_mod){

    uint32_t aux_u32;

    switch(*last_loaded_sec){
        case WP_WSA_BIN_MOD_SEC_ID_CUSTOM:
            goto start_at_type_sec;
            break;
        case WP_WSA_BIN_MOD_SEC_ID_TYPE:
            goto start_at_import_sec;
            break;
        case WP_WSA_BIN_MOD_SEC_ID_IMPORT:
            goto start_at_function_sec;
            break;
        case WP_WSA_BIN_MOD_SEC_ID_FUNCTION:
            goto start_at_table_sec;
            break;
        case WP_WSA_BIN_MOD_SEC_ID_TABLE:
            goto start_at_memory_sec;
            break;
        case WP_WSA_BIN_MOD_SEC_ID_MEMORY:
            goto start_at_global_sec;
            break;
        case WP_WSA_BIN_MOD_SEC_ID_GLOBAL:
            goto start_at_export_sec;
            break;
        case WP_WSA_BIN_MOD_SEC_ID_EXPORT:
            goto start_at_start_sec;
            break;
        case WP_WSA_BIN_MOD_SEC_ID_START:
            goto start_at_element_sec;
            break;
        case WP_WSA_BIN_MOD_SEC_ID_ELEMENT:
            goto start_at_data_count_sec;
            break;
        case WP_WSA_BIN_MOD_SEC_ID_DATA_COUNT:
            goto start_at_code_sec;
            break;
        case WP_WSA_BIN_MOD_SEC_ID_CODE:
            goto start_at_data_sec;
            break;
        case WP_WSA_BIN_MOD_SEC_ID_DATA:
            goto start_at_final_custom_sec;
            break;
        default:
            return NULL;
    }

    start_at_type_sec:        
    if(section_id == WP_WSA_BIN_MOD_SEC_ID_TYPE){
        // Type Section 
        index = LoadBinSection(index, &bin_mod->typesec);
        if (!index){                
            return NULL;                                        
        }
        *last_loaded_sec = WP_WSA_BIN_MOD_SEC_ID_TYPE;
        return index;
    }

    start_at_import_sec:
    if(section_id ==  WP_WSA_BIN_MOD_SEC_ID_IMPORT){
        // import Section 
        index = LoadBinSection(index, &bin_mod->importsec);
        if (!index){                
            return NULL;                                        
        }
        *last_loaded_sec = WP_WSA_BIN_MOD_SEC_ID_IMPORT;
        return index;
    }

    start_at_function_sec:        
    if(section_id ==  WP_WSA_BIN_MOD_SEC_ID_FUNCTION){
        // Function Section 
        index = LoadBinSection(index, &bin_mod->functionsec);
        if (!index){                
            return NULL;                                        
        }
        *last_loaded_sec = WP_WSA_BIN_MOD_SEC_ID_FUNCTION;
        return index;
    }

    start_at_table_sec:       
    if(section_id == WP_WSA_BIN_MOD_SEC_ID_TABLE){
        // Table Section
        index = LoadBinSection(index, &bin_mod->tablesec);
        if (!index){                
            return NULL;                                        
        }   
        *last_loaded_sec = WP_WSA_BIN_MOD_SEC_ID_TABLE;
        return index;
    }

    start_at_memory_sec:
    if(section_id == WP_WSA_BIN_MOD_SEC_ID_MEMORY){
        // Memory Section
        index = LoadBinSection(index, &bin_mod->memsec);
        if (!index){                
            return NULL;                                        
        } 
        *last_loaded_sec = WP_WSA_BIN_MOD_SEC_ID_MEMORY;
        return index;
    }

    start_at_global_sec:
    if(section_id == WP_WSA_BIN_MOD_SEC_ID_GLOBAL){
        // GLOBAL Section
        index = LoadBinSection(index, &bin_mod->globalsec);
        if (!index){                
            return NULL;                                        
        } 
        *last_loaded_sec = WP_WSA_BIN_MOD_SEC_ID_GLOBAL;
        return index;
    }

    start_at_export_sec:
    if(section_id == WP_WSA_BIN_MOD_SEC_ID_EXPORT){
        //Export Section                
        index = LoadBinSection(index, &bin_mod->exportsec);
        if (!index){                
            return NULL;                                        
        } 
        *last_loaded_sec = WP_WSA_BIN_MOD_SEC_ID_EXPORT;
        return index;   
    }

    start_at_start_sec:
    if(section_id == WP_WSA_BIN_MOD_SEC_ID_START){
        // Start Section
        index = LoadBinSection(index, &bin_mod->startsec);
        if (!index){                
            return NULL;                                        
        } 
        *last_loaded_sec = WP_WSA_BIN_MOD_SEC_ID_START;
        return index;  
    }

    start_at_element_sec:
    if(section_id == WP_WSA_BIN_MOD_SEC_ID_ELEMENT){
        // Element Section              
        index = LoadBinSection(index, &bin_mod->elemsec);
        if (!index){                
            return NULL;                                        
        } 
        *last_loaded_sec = WP_WSA_BIN_MOD_SEC_ID_ELEMENT;
        return index;      
    }

    start_at_data_count_sec:
    if(section_id == WP_WSA_BIN_MOD_SEC_ID_DATA_COUNT){
        //Data Count Section   
        index = LoadBinSection(index, &bin_mod->datacountsec);
        if (!index){                
            return NULL;                                        
        } 
        *last_loaded_sec = WP_WSA_BIN_MOD_SEC_ID_DATA_COUNT;
        return index;
    }

    start_at_code_sec:
    if(section_id == WP_WSA_BIN_MOD_SEC_ID_CODE){
        //Code Section   
        index = LoadBinSection(index, &bin_mod->codesec);
        if (!index){                
            return NULL;                                        
        } 
        *last_loaded_sec = WP_WSA_BIN_MOD_SEC_ID_CODE;
        return index;
    }

    start_at_data_sec:
    if(section_id == WP_WSA_BIN_MOD_SEC_ID_DATA){
        //Data Section 
        index = LoadBinSection(index, &bin_mod->datasec);
        if (!index){                
            return NULL;                                        
        } 
        *last_loaded_sec = WP_WSA_BIN_MOD_SEC_ID_DATA;
        return index;    
    }

    start_at_final_custom_sec:    
    if(section_id == WP_WSA_BIN_MOD_SEC_ID_CUSTOM){                
        //custom section will be ignored 
        // Has no use on module execution.
        index = DecodeLeb128UInt32(index, &aux_u32);
        if (!index){                
            return NULL;                                        
        } 
        index = index + aux_u32;
        return index;
    }

    return NULL;
}

/**
 * @brief Function thar create a WasmbinModule from byte array.
 * 
 * @param bin_mod reference to target module.
 * @param buf Pointer to wasm binary location
 * @param size Len in bytes for wasm binary
 * 
 * @return WpObjectResult 
 */
WpObjectResult LoadWasmBuffer(const uint8_t * const buf, const uint32_t size, WasmBinModule *bin_mod) {

    WpObjectResult result;
    ObjectResultInit(&result);

    const uint8_t *index = buf;                             // pointer to byte to traverse the binary file
    //const uint8_t *pos_mark;                              // auxiliary pointer to calc len
    const uint8_t *buf_end = buf + size;                    // pointer to end of binary module
    //uint8_t section_counter = 0;                          // auxiliary variable for avoid endless loop
    
    uint32_t aux_u32 = 0;                                   // auxiliary var to store u32 values
    uint8_t last_loaded_section = 0;                        // var to keep track section order.
    uint8_t section_id;
    
    bin_mod->bin_len = size;
    bin_mod->start = index;

    
    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    // Check minimun buffer length for magic and version number ///////////////////////////////////
    if(size < 8){
        //if buffer's size is less than 4 is a wrong binary module
        ObjectResultAddError(&result, WP_DIAG_ID_INVALID_MODULE_SIZE, W_DIAG_MOD_LIST_LOADER);
        #if WASPC_CONFIG_DEV_FLAG == 1
        //TODO
        #endif
        
        return result;
    }

    // Check magic number /////////////////////////////////////////////////////////////////////////    
    if (!ValidateMagic(index)){
        //invalid magic number
        ObjectResultAddError(&result, WP_DIAG_ID_INVALID_MAGIC, W_DIAG_MOD_LIST_LOADER);
        #if WASPC_CONFIG_DEV_FLAG == 1
        //TODO
        #endif

        return result;
    }   
    index = index + 4; 
    ///////////////////////////////////////////////////////////////////////////////////////////////

    // Check version number /////////////////////////////////////////////////////////////////////////    
    if (!ValidateVersion(index, &aux_u32)){
        //invalid version
        ObjectResultAddError(&result, WP_DIAG_ID_INVALID_VERSION, W_DIAG_MOD_LIST_LOADER);
        #if WASPC_CONFIG_DEV_FLAG == 1
        //TODO
        #endif

        return result;    
    }
    index = index + 4;
    bin_mod->version = aux_u32;
    ///////////////////////////////////////////////////////////////////////////////////////////////

    //Traversing the binary file 
    while(NOT_END()){
        //Seccion
        section_id = READ_BYTE();        
        //
        index = LoadBinSectionById(index, section_id, &last_loaded_section, bin_mod);
        if (!index){  
            //invalid encoded
            ObjectResultAddError(&result, WP_DIAG_ID_UNEXPECTED_SECTION_ID, W_DIAG_MOD_LIST_LOADER);
            #if WASPC_CONFIG_DEV_FLAG == 1
            //TODO
            #endif
                    
            return result;                       
        }        
    }
    
    return result;

    #undef READ_BYTE
    #undef NOT_END

    
}