#include "objects/export.h"

void WpExportInstanceInit(WpExportInstance *self) {
    self->wp_type = WP_OBJECT_EXPORT_INSTANCE;
    self->name_len = 0;
    self->name = NULL;
    self->export_type = 0;
    self->address = NULL;
    self->next = NULL;
}