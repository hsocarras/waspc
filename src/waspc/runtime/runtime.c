/**
 * @file runtime.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-01-22
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "runtime/runtime.h"
#include "objects/object.h"
#include "validation/wasm_validator.h"


#include <assert.h>
#include <string.h>
#include <stdlib.h>
//#include <stdio.h>

/**
 * @brief Init function for runtime state object
 * 
 * @param self 
 */
void WpRuntimeInit(WpRuntimeState *self){
    
    // Init code mem relate properties
    self->code_mem_start = NULL;
    self->code_mem_ptr = NULL;
    self->code_mem_size = 0;

    // Init error object
    WpErrorInit(&self->err);

    //Init validator
    WpValidatorStateInit(&self->validator);

    // Init modules hash table
    HashTableInit(&self->modules);    
}

/**
 * @brief Function to init code memory area
 * 
 * @param self 
 * @param start pointer to where memory start
 * @param mem_size memory size in bytes
 */
void WpRuntimeCodeMemInit(WpRuntimeState *self, const uint8_t *start, uint32_t mem_size){

    // Init code memory
    self->code_mem_start = start;
    self->code_mem_ptr = start;
    self->code_mem_size = mem_size;
}

/**
 * @brief Function to get the binary modulo into the runtime memory
 * 
 * @param self 
 * @param mod_name every module must have a name to be identified.
 * @param buffer_start address of the buffer where the module is stored.
 * @param mod_size module size in bytes.
 * @return WpObject* 
 */
WpObject * WpRuntimeReadModule(WpRuntimeState *self, const char * mod_name, const uint8_t *buffer_start, uint32_t mod_size){

    //first check that code memory is not null
    assert(self->code_mem_start);

    //check that hash table was initilised corectly.    
    if(self->modules.capacity == 0){
        self->err.id = 1;
        #if WASPC_CONFIG_DEV_FLAG == 1
        strcpy_s(self->err.file, 64,"runtime/runtime.c"); 
        strcpy_s(self->err.func, 32,"WpRuntimeLoadModule");
        #endif
        return (WpObject *)&self->err;
    }

    //check that are enought memory.
    uint32_t memory_usage = self->code_mem_ptr - self->code_mem_start;
    uint32_t memory_left = self->code_mem_size - memory_usage;
    if(mod_size > memory_left){
        self->err.id = 2;
        #if WASPC_CONFIG_DEV_FLAG == 1
        strcpy_s(self->err.file, 64,"runtime/runtime.c"); 
        strcpy_s(self->err.func, 32,"WpRuntimeLoadModule");
        #endif
        return (WpObject *)&self->err;
    }

    const uint8_t *mod_start_address = memcpy(self->code_mem_ptr, buffer_start, mod_size);
    self->code_mem_ptr += mod_size;         //move pointer to next free memory
    
    //create module object
    WpModuleState *mod = (WpModuleState *)malloc(sizeof(WpModuleState));
    WpModuleInit(mod);

    if(!mod){
        self->err.id = 3;
        #if WASPC_CONFIG_DEV_FLAG == 1
        strcpy_s(self->err.file, 64,"runtime/runtime.c"); 
        strcpy_s(self->err.func, 32,"WpRuntimeLoadModule");
        #endif
        return (WpObject *)&self->err;
    }
    
    //register module object
    assert(mod_name);    
    WpModuleState *curent_mod = (WpModuleState *)HashTableSet(&self->modules, mod_name, mod);
    
    if(!curent_mod){
        self->err.id = 4;
        #if WASPC_CONFIG_DEV_FLAG == 1
        strcpy_s(self->err.file, 64,"runtime/runtime.c"); 
        strcpy_s(self->err.func, 32,"WpRuntimeLoadModule");
        #endif
        return (WpObject *)&self->err;
    }

    curent_mod->name = mod_name;
    curent_mod->status = WP_MODULE_STATUS_READ;
    curent_mod->buf = mod_start_address;
    curent_mod->bufsize = mod_size;

    return (WpObject *)curent_mod;

}


WpObject * WpRuntimeValidateModule(WpRuntimeState *self, WpModuleState *mod){ 
    
    if(!mod){
        self->err.id = 11;
        #if WASPC_CONFIG_DEV_FLAG == 1
        strcpy_s(self->err.file, 64,"runtime/runtime.c"); 
        strcpy_s(self->err.func, 32,"WpRuntimeValidateModule");
        #endif
        return (WpObject *)&self->err;
    }
        
    

    const uint8_t *index = mod->buf;                                // pointer to byte to traverse the binary file    
    const uint8_t *buf_end = index + mod->bufsize;                  // pointer to end of binary module
    uint32_t decoded_u32 = 0;                                       // auxiliary var to store u32 values
    uint8_t section_id;
    uint8_t last_loaded_section = 0;                                // var to keep track section order.

    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    // Check minimun module size for magic and version number ///////////////////////////////////
    if(mod->bufsize < 12){
        self->err.id = 5;
        mod->status = WP_MODULE_STATUS_INVALID;
        #if WASPC_CONFIG_DEV_FLAG == 1
        strcpy_s(self->err.file, 64,"runtime/runtime.c"); 
        strcpy_s(self->err.func, 32,"WpRuntimeValidateModule");
        #endif
        return (WpObject *)&self->err;
    }

    // Check magic number /////////////////////////////////////////////////////////////////////////    
    if (!ValidateMagic(index)){
        self->err.id = 13;
        mod->status = WP_MODULE_STATUS_INVALID;
        #if WASPC_CONFIG_DEV_FLAG == 1
        strcpy_s(self->err.file, 64,"runtime/runtime.c"); 
        strcpy_s(self->err.func, 32,"WpRuntimeValidateModule");
        #endif
        return (WpObject *)&self->err;
    }   
    index = index + 4; 
    ///////////////////////////////////////////////////////////////////////////////////////////////

    // Check version number /////////////////////////////////////////////////////////////////////////    
    if (!ValidateVersion(index, &decoded_u32)){
        self->err.id = 14;
        mod->status = WP_MODULE_STATUS_INVALID;
        #if WASPC_CONFIG_DEV_FLAG == 1
        strcpy_s(self->err.file, 64,"runtime/runtime.c"); 
        strcpy_s(self->err.func, 32,"WpRuntimeValidateModule");
        #endif
        return (WpObject *)&self->err;    
    }
    index = index + 4;    
    mod->version = decoded_u32;
    ///////////////////////////////////////////////////////////////////////////////////////////////

    //Traversing the binary file 
    while(NOT_END()){
        //Seccion
        section_id = READ_BYTE();        
        //
        index = ValidateBinSectionById(index, section_id, &last_loaded_section, mod);
        if (!index){  
            self->err.id = 16;
            mod->status = WP_MODULE_STATUS_INVALID;
            #if WASPC_CONFIG_DEV_FLAG == 1
            strcpy_s(self->err.file, 64,"runtime/runtime.c"); 
            strcpy_s(self->err.func, 32,"WpRuntimeValidateModule");
            #endif
            return (WpObject *)&self->err;                                                             
        }        
    }
    
   

    #undef READ_BYTE
    #undef NOT_END

    mod->status = WP_MODULE_STATUS_VALIDATED;
    return (WpObject *)mod;
}