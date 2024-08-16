

#include "diagnostic/error.h"

WpError CreateError(WpErrorId id){

    WpError err;

    err.head.type = ERROR;
    err.id = OK;
}