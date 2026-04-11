/**
 * @file export.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-02-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef WASPC_OBJECTS_EXPORT_H
#define WASPC_OBJECTS_EXPORT_H

#ifdef __cplusplus
    extern "C" {
#endif

//wasp includes
#include "objects/object.h"

#include <stdint.h>

typedef struct WpExportInstance {
    /// head for all Waspc object to allow cast
    WpObjectType wp_type;

    uint32_t name_len;
    const uint8_t *name;
    uint8_t export_type;     //0 for func, 1 for global, 2 for memory
    uint8_t *address;
    
    struct WpExportInstance *next;

}WpExportInstance;


// Methods **************************************************************************************************

void WpExportInstanceInit(WpExportInstance *self);

#ifdef __cplusplus
    }
#endif

#endif
