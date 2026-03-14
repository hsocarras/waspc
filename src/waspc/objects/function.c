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

 void WpFunctionInstanceInit(WpFunctionInstance *self){

    self->wp_type = WP_OBJECT_FUNCTION_INSTANCE;
    self->module = NULL;
    self->param_len = 0;
    self->param_types = NULL;
    self->ret_len = 0;
    self->ret_types = NULL;
    self->locals = NULL;
    self->body = NULL;

    self->next = NULL;
 
 }