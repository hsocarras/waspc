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
 * @param id  Diagnostic Id.
 * @param mod Module that produce the error
 */
void WpErrorInit(WpError *self){
    
    self->type = WP_OBJECT_ERROR;

    self->id = 0;
    
    //#if WASPC_CONFIG_DEV_FLAG == 1    
    strcpy_s(self->file, 64, "none"); 
    strcpy_s(self->func, 32, "nan");    
    //#endif
    
}