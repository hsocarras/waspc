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

#include "loader/wasm_validator.h"
#include "runtime/runtime.h"
#include "webassembly/validation/contex.h"
#include "webassembly/structure/module.h"
#include "webassembly/structure/types.h"

#include <stdint.h>

/**
 * @brief Limits must have meaningful bounds that are within a given range
 * 
 * @param li Limit value to validate
 * @param k Bound to check
 * @return uint8_t 0-OK, 1-Invalid min, 2-Invalid max
 */
static uint8_t ValidateTypeLimits(Limits *li, uint32_t k){    

    if(li->min >= k){
        if(li->max >0){
            if(li->max <= k & li->max >= li->min){
                //valid
                return 0;
            }
            else{
                //not valid max value
                return 2;
            }

        }        
    }
    else{
        //not valid min value        
        return 1;
    }
}


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
 */
static WpError ValidateModTable(WasmModule *mod){

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

}