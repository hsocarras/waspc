
#include "objects/global.h"

void WpGlobalInstanceInit(WpGlobalInstance *self){

    self->wp_type = WP_OBJECT_GLOBAL_INSTANCE;
    self->mut =  0;
    self->type = WAS_EX_VAL_TYPE_NULL;
    self->val = (StackValue){0};

    self->next = NULL;
}