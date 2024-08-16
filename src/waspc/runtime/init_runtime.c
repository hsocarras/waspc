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


/**
 * @brief Runtime constructor.
 * 
 * @param self 
 */
void InitRuntime(RuntimeEnv *self){

    //calling load_mem_manager object's constructor
    InitLoadMemoryManager(&self->load_mem_manager, self->load_memory, LOAD_MEMORY_SIZE);

    //Initialice block_loaded properties///////////////////////////////////////////////
    strcpy_s(self->block_loaded[0].name, 1, "");    
    self->block_loaded[0].size = 0;
    self->block_loaded[0].start = NULL;

    strcpy_s(self->block_loaded[1].name, 1, "");
    self->block_loaded[1].size = 0;
    self->block_loaded[1].start = NULL;
    ////////////////////////////////////////////////////////////////////////////////////

}