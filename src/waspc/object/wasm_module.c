/**
 * @file wasm_module.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-10-11
 * 
 * @copyright Copyright (c) 2024
 * 
 */

 #include "object/wasm_module.h"

 void ObjectWasmModuleInit(WasmModule *self){

    self->type = WP_OBJECT_WASM_MODULE;

    // type  
    self->type_len = 0;    
    self->types = NULL;

    // Functions
    self->func_len = 0;
    self->funcs = NULL;

    //Tables
    self->table_len = 0;
    self->tables = NULL;

    //Mems
    self->mem_len = 0;
    self->mems = NULL;

    //Global
    self->global_len = 0;
    self->globals = NULL;

    //Element
    self->elem_len = 0;
    self->elems = NULL; 

    //Data
    self->data_count = 0;
    self->datas = NULL;

    //start
    self->start = 0;

    // import
    self->imports_len = 0;
    self->imports = NULL;   

    //Export
    self->exports_len = 0;
    self->exports = NULL;   

 }