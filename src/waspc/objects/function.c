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

    self->type = WP_OBJECT_FUNCTION_INSTANCE;
    self->module = NULL;
    self->code = NULL;
 }