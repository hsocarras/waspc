/**
 * @file module_instance.h
 * @author Hector E. Socarras (hsocarras1987@gmail.com)
 * @brief Header file for webassembly module instances related to spec's runtime chapter
 * @version 0.1
 * @date 2024-06-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef WASPC_WEBASSEMBLY_RUNTIME_MODULE_INSTANCE_H
#define WASPC_WEBASSEMBLY_RUNTIME_MODULE_INSTANCE_H

#ifdef __cplusplus
    extern "C" {
#endif

#include "webassembly/runtime/export_instance.h"

#include <stdint.h>

/**
 * @brief A module instance is the runtime representation of a module. It is created by instantiating a module, and collects
 * runtime representations of all entities that are imported, defined, or exported by the module.
 *
 * moduleinst ::= { types functype*,
 *    funcaddrs funcaddr*,
 *  tableaddrs tableaddr*,
 *   memaddrs memaddr*,
 *   globaladdrs globaladdr*,
 *   elemaddrs elemaddr*,
 *   dataaddrs dataaddr*,
 *   exports exportinst* }
 * 
 */
typedef struct {

    uint32_t export_instance_count;
    ExportInstance exports;

} ModuleInstance;


#ifdef __cplusplus
    }
#endif

#endif