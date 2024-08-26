

#include "diagnostic/error.h"

WpError CreateError(WpErrorId id, WpErrorModuleList mod, uint32_t func, uint32_t place){

    WpError err;

    err.head.type = WP_OBJECT_ERROR;
    err.id = id;

    err.detail.origin = mod;
    err.detail.func = func;
    err.detail.place = place;

    return err;
}