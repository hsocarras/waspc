/**
 * @file function.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-02-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */

 #include "objects/function.h"

 /**
 * @brief Default constructor for WpFunctionInstance objects
 * 
 * @param self Pointer to the WpFunctionInstance object to initialize
 * @param func_kind The type of the function
 */
 void WpFunctionInstanceInit(WpFunctionInstance *self, WpFunctionType func_kind) {

    self->wp_type = WP_OBJECT_FUNCTION_INSTANCE;
    self->next = NULL;

    self->func_kind = func_kind;
    self->address = NULL;
    self->host_func = NULL;

    self->module = NULL;

    self->func_type = NULL;
    
    
    self->locals = NULL;
    self->body = NULL;
    self->body_end = NULL;
 
 }