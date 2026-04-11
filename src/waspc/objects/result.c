#include "objects/result.h"

void WpResultInit(WpResult *self) {
    self->wp_type = WP_OBJECT_RESULT;
    self->vals = NULL;
    self->len = 0;
}