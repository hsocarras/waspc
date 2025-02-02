/**
 * @file wasm_validator.c
 * @author your name (you@domain.com)
 * @brief 
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
#include "utils/leb128.h"

#include <stdint.h>

/**
 * @brief Inner function to read a binary section into a WasmBinSection structure 
 * 
 * @param index 
 * @param sec 
 * @return const uint8_t* index pointing at next section or NULL if error
 */
static const uint8_t * ReadBinSection(const uint8_t *index, WasmBinSection *sec){

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

void WpValidatorStateInit(WpValidatorState *self){

    self->c.type_len = 0;
    self->c_prime.type_len = 0;
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
 * @brief Function to validate wasm binary magic number
 *          magic ::= 0x00 0x61 0x73 0x6D
 * @param buf wasm binary format 
 * @return uint8_t 1 - ok, 0 - invalid 
 */
uint8_t ValidateMagic(const uint8_t *buf){

   
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
uint8_t ValidateVersion(const uint8_t *buf, uint32_t *version_number){
    
    

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
 * @brief Limits must have meaningful bounds that are within a given range
 * 
 * @param li Limit value to validate
 * @param k Bound to check
 * @return uint8_t 0-Invalid, 1-valid
 */
static uint8_t ValidateLimitsType(const Limits *li, uint32_t k){    

    if(li->min >= k){
        if(li->max >0){
            if(li->max <= k & li->max >= li->min){
                //valid
                return 1;
            }
            else{
                //not valid max value
                return 0;
            }

        }        
    }
    else{
        //not valid min value        
        return 0;
    }
    return 0;
}

/**
 * @brief Validate table type
 * The limits limits must be valid within range 2^32 − 1.
 * 
 * @param t table type to validate
 * @return uint8_t 0-Invalid, 1-valid
 *
static uint8_t ValidateTableType(const TableType *t){    

    return ValidateLimitsType(&t->lim, 0xFFFFFFFF);
}*/

/**
 * @brief Validate Memory type
 * The limits limits must be valid within range 2^16 − 1.
 * 
 * @param m Memory type to validate
 * @return uint8_t 0-Invalid, 1-valid
 *
static uint8_t ValidateTableType(const MemType *m){    

    return ValidateLimitsType(m, 0xFFFF);
}*/




/**
 * @brief 
 * 
 * @param imports Module imports to validate
 * @param imports_len Import's lenght
 * @param c Context 
 * @return WpError 
 
static WpObjectResult ValidateImportSection(Import *imports, uint32_t imports_len){

    WpObjectResult result;
    InitObjectResult(&result);

    ImportDesc desc;

    for(int i=0; i<imports_len; i++){

        //first get import mod_name to get the context

        
        desc = imports[i].desc;

        switch(desc->type){

            case WP_WAS_STRUC_MOD_IMPORT_DESC_TYPE_FUNC:
                if(desc->idx < c.type_len){

                }
                else{
                    
                }
                break;
            case WP_WAS_STRUC_MOD_IMPORT_DESC_TYPE_TABLE:
                break;
            case WP_WAS_STRUC_MOD_IMPORT_DESC_TYPE_MEM:
                break
            case WP_WAS_STRUC_MOD_IMPORT_DESC_TYPE_GLOBAL:
                break;
            default:

        }
    }

}*/

/**
 * @brief Under the context 𝐶′:
 * – For each table𝑖 in module.tables, the definition table𝑖 must be valid with a table type tt𝑖.
 * 
 * @param mod 
 * @return WpError 
 *
static WpError ValidateTableSection(WasmModule *mod){

    uint32_t i;
    Table t;

    for(i = 0; i < mod->table_len; i++){

        t = mod->tables[i];

    }
}


WpError ValidateModule(RuntimeEnv *self, WasmModule *mod){

    WpError result = CreateError(WP_DIAG_ID_OK, W_DIAG_MOD_LIST_DECODER, 253, 0);       //No error
    
    WasContext c, c_prime;

    //𝐶.types is module.types
    c.type_len = mod->type_len;
    c.types = mod->types;
    c_prime.type_len = c.type_len;
    c_prime.types = c.types;

    //section function:

    WpError = ValidateModFunction();
    

    return result;

}*/

/**
 * @brief Function to validate each section inside a module.
 * 
 * @param index 
 * @param section_id 
 * @param previous_secction 
 * @param mod 
 * @return const uint8_t* 
 */
const uint8_t* ValidateBinSectionById(const uint8_t *index, const uint8_t section_id, uint8_t *previous_secction, WpModuleState *mod){

    uint32_t aux_u32;                                                    //auxiliary var to store u32 values
    /**
     * @brief Sections inside a module are encoded in a specific order.
     * this switch case is to validate the order of the sections.
     * 
     */
    switch(*previous_secction){
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
        index = ReadBinSection(index, &mod->typesec);        
        if (!index){                
            return NULL;                                        
        }
        VecFuncType *types = DecodeTypeSection(mod);
        if(!types){
            return NULL;
        }
        ///Types are always valid Wev Assembly Spec 3.2.3
        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_TYPE;
        return index;
    }

    start_at_import_sec:
    if(section_id ==  WP_WSA_BIN_MOD_SEC_ID_IMPORT){
        // import Section 
        index = ReadBinSection(index, &mod->importsec);
        if (!index){                
            return NULL;                                        
        }
        VecImport *imports = DecodeImportSection(mod);
        if(!imports){
            return NULL;
        }
        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_IMPORT;
        return index;
    }

    start_at_function_sec:        
    if(section_id ==  WP_WSA_BIN_MOD_SEC_ID_FUNCTION){
        // Function Section 
        index = ReadBinSection(index, &mod->functionsec);
        if (!index){                
            return NULL;                                        
        }
        VecFunc *funcs = DecodeFunctionSection(mod);
        if(!funcs){
            return NULL;
        }
        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_FUNCTION;
        return index;
    }

    start_at_table_sec:       
    if(section_id == WP_WSA_BIN_MOD_SEC_ID_TABLE){
        // Table Section
        index = ReadBinSection(index, &mod->tablesec);
        if (!index){                
            return NULL;                                        
        }   
        VecTable *tables = DecodeTableSection(mod);
        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_TABLE;
        return index;
    }

    start_at_memory_sec:
    if(section_id == WP_WSA_BIN_MOD_SEC_ID_MEMORY){
        // Memory Section
        index = ReadBinSection(index, &mod->memsec);
        if (!index){                
            return NULL;                                        
        } 
        VecMem *mems = DecodeMemSection(mod);
        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_MEMORY;
        return index;
    }

    start_at_global_sec:
    if(section_id == WP_WSA_BIN_MOD_SEC_ID_GLOBAL){
        // GLOBAL Section
        index = ReadBinSection(index, &mod->globalsec);
        if (!index){                
            return NULL;                                        
        } 
        VecGlobal *globals = DecodeGlobalSection(mod);
        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_GLOBAL;
        return index;
    }

    start_at_export_sec:
    if(section_id == WP_WSA_BIN_MOD_SEC_ID_EXPORT){
        //Export Section                
        index = ReadBinSection(index, &mod->exportsec);
        if (!index){                
            return NULL;                                        
        } 
        VecExport *exports = DecodeExportSection(mod);
        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_EXPORT;
        return index;   
    }

    start_at_start_sec:
    if(section_id == WP_WSA_BIN_MOD_SEC_ID_START){
        // Start Section
        index = ReadBinSection(index, &mod->startsec);
        if (!index){                
            return NULL;                                        
        } 
        uint32_t *start = DecodeStartSection(mod);
        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_START;
        return index;  
    }

    start_at_element_sec:
    if(section_id == WP_WSA_BIN_MOD_SEC_ID_ELEMENT){
        // Element Section              
        index = ReadBinSection(index, &mod->elemsec);
        if (!index){                
            return NULL;                                        
        } 
        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_ELEMENT;
        return index;      
    }

    start_at_data_count_sec:
    if(section_id == WP_WSA_BIN_MOD_SEC_ID_DATA_COUNT){
        //Data Count Section   
        index = ReadBinSection(index, &mod->datacountsec);
        if (!index){                
            return NULL;                                        
        } 
        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_DATA_COUNT;
        return index;
    }

    start_at_code_sec:
    if(section_id == WP_WSA_BIN_MOD_SEC_ID_CODE){
        //Code Section   
        index = ReadBinSection(index, &mod->codesec);
        if (!index){                
            return NULL;                                        
        } 
        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_CODE;
        return index;
    }

    start_at_data_sec:
    if(section_id == WP_WSA_BIN_MOD_SEC_ID_DATA){
        //Data Section 
        index = ReadBinSection(index, &mod->datasec);
        if (!index){                
            return NULL;                                        
        } 
        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_DATA;
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