/**
 * @file export_instance.h
 * @author Hector E. Socarras (hsocarras1987@gmail.com)
 * @brief Header file for webassembly export instances related to spec's runtime chapter
 * @version 0.1
 * @date 2024-06-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef WASPC_WEBASSEMBLY_RUNTIME_EXPORT_INSTANCE_H
#define WASPC_WEBASSEMBLY_RUNTIME_EXPORT_INSTANCE_H

#ifdef __cplusplus
    extern "C" {
#endif

#include <stdint.h>




/**
 * @brief An export instance is the runtime representation of an export. It defines the export’s name and the associated
 * external value.
 * exportinst ::= {name name, value externval}
 */
typedef struct {
    char *name;
    //ExternVal value;
}ExportInstance;


#ifdef __cplusplus
    }
#endif

#endif