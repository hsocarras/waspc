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

/**
 * @brief WpExportInstance represents an export in a WebAssembly module. It contains the export's name, type, and address.
 */
typedef struct WpExportInstance {
    /// head for all Waspc object to allow cast
    WpObjectType wp_type;

    uint32_t name_len;          // The length of the export name in bytes
    const uint8_t *name;        // The name of the export as a UTF-8 string
    uint8_t export_type;        //0 for func, 1 for global, 2 for memory
    WpObject *address;           // pointer inside store where de exported item is located, 
                                // for func it's the pointer to the function instance, 
                                // for global it's the pointer to the global instance, 
                                // for memory it's the pointer to the memory instance
    
    struct WpExportInstance *next;

}WpExportInstance;


// Methods **************************************************************************************************

void WpExportInstanceInit(WpExportInstance *self);

#ifdef __cplusplus
    }
#endif

#endif
