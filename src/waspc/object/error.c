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

#include "object/error.h"


/**
 * @brief Default constructor for Error objects
 * 
 * @param self
 * @param id  Diagnostic Id.
 * @param mod Module that produce the error
 */
void ObjectErrorInit(WpObjectError *self, WpDiagId id, WpDiagModuleList mod){

    
    self->type = WP_OBJECT_ERROR;

    self->id = id;

    self->mod = mod;
    
    #if WASPC_CONFIG_DEV_FLAG == 1
    uint16_t func = 0;
    #endif
    
}