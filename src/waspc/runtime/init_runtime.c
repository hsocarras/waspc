/**
 * @file init_runtime.c
 * @author Hector E. Socarras (hsocarras1987@gmail.com)
 * @brief  File with funtion's implementation for initialize waspc runtime and setup all necesary things.
 * @version 0.1
 * @date 2024-08-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */

//Wasp includes 
#include "runtime/runtime.h"

#include <string.h>
#include <stdio.h>

/**
 * @brief Runtime constructor.
 * 
 * @param self 
 */
void InitRuntime(RuntimeEnv *self){

    self->in = NULL;
    self->input_size = 0;
    self->out = NULL;
    self->output_size = 0;
    self->mark = NULL;
    self->mark_size = 0;
    self->code_mem_size = 0;
    self->code_mem = NULL;
    
    //Init hash table
    InitHashTable(&self->table_wasm_bin, 10);
    
    //calling load_mem_manager object's constructor
    //InitWorkMemory(&self->work_mem);
    /*
    //Initialice wasm_loaded properties///////////////////////////////////////////////
    self->wasm_loaded[0].id = 0;    
    self->wasm_loaded[0].size = 0;
    self->wasm_loaded[0].start = NULL;

    self->wasm_loaded[1].id = 0;  
    self->wasm_loaded[1].size = 0;
    self->wasm_loaded[1].start = NULL;
    ////////////////////////////////////////////////////////////////////////////////////
    */

}
