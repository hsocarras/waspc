
#include "objects/was_deftype.h"

void WpWasDefTypeInit(WpWasDefType *self, WpWasDefTypeEnum def_type){
    self->wp_type = WP_OBJECT_DEFINED_TYPE;
    self->next = NULL;
    self->def_type = def_type;
    self->binary = NULL;
    //TODO implement the other recursive types.
    switch (def_type)
    {
    case WP_WAS_DEF_TYPE_FUNC_TYPE:
        self->def.func_type.param_len = 0;
        self->def.func_type.param_types = NULL;
        self->def.func_type.ret_len = 0;
        self->def.func_type.ret_types = NULL;
        break;
    default:
        break;
    }
}