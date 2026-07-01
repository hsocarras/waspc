/**
 * @file module_instance.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-01-26
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "objects/module_instance.h"
#include <stdlib.h>


/**
 * @version 3.0
 * @brief Initialize a module state object
 */
void WpModuleInstanceInit(WpModuleInstance *self){

    self->wp_type = WP_OBJECT_MODULE_INSTANCE;    
    ////////////////////////////////////////////////////////////////////////////////////////
    
    self->import_count = 0;
    self->table_count = 0;
    self->tag_count = 0;
    self->element_count = 0;
    self->data_count = 0;

    //////////////////////////////////////////////////////////////////////////////////////
    self->types = NULL;
    self->type_count = 0;
    self->globals = NULL;
    self->global_count = 0;
    self->funcs = NULL;
    self->function_count = 0;
    self->mems = NULL;
    self->mem_count = 0;
    self->exports = NULL;
    self->export_count = 0;

    //Start function index.
    self->start_func_index = 0xFFFFFFFF;

    //Next pointer for store linked list
    self->next = NULL;
    
}

