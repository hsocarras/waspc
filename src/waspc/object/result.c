/**
 * @file result.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-09-15
 * 
 * @copyright Copyright (c) 2024
 * 
 */


#include "object/result.h"

/**
 * @brief Default constructor for WpObjectResult
 * 
 * @param self 
 */
void ObjectResultInit(WpObjectResult *self){

    self->type = WP_OBJECT_RESULT;

    self->result_type = WP_OBJECT_RESULT_TYPE_VALID;

    self->value.u32 = 0;
}

/**
 * @brief Function to add a error object to value property.
 * 
 * @param self 
 * @param id Diagnostic Id.
 * @param mod Module that produce the error
 */
void ObjectResultAddError (WpObjectResult *self, WpDiagId id, WpDiagModuleList mod){

    WpObjectError err;
    ObjectErrorInit(&self->value.err, id, mod);

}