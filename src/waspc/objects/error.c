/**
 * @file error.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "objects/error.h"

#include <string.h>

/**
 * @brief Default constructor for Error objects
 * 
 * @param self
 */
void WpErrorInit(WpError *self){
    
    self->wp_type = WP_OBJECT_ERROR;
    
    /// errors id, unique for every error
    self->error_type = 0;
    self->id = 0;
    self->timestamp = 0;
    self->msg = NULL;

    #if WASPC_CONFIG_DEV_FLAG == 1
    self->module_id = 0;
    self->func_code = 0;
    self->block_code = 0;
    self->err_code = 0;
    #endif
    
}