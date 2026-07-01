#include "objects/export.h"

/**
 * @brief Default constructor for WpExportInstance objects
 * 
 * @param self Pointer to the WpExportInstance object to initialize
 */
void WpExportInstanceInit(WpExportInstance *self) {
    self->wp_type = WP_OBJECT_EXPORT_INSTANCE;
    self->name_len = 0;
    self->name = NULL;
    self->export_type = 0;
    self->address = NULL;
    self->next = NULL;
}