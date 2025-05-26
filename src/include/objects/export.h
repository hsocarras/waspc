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
#include "objects/function.h"
#include "webassembly/execution/runtime/values.h"
#include "webassembly/structure/module.h"

#include <stdint.h>

typedef struct WpExportInstance {
    /// head for all Waspc object to allow cast
    WpObjectType type;
    /// @brief name of the export
    Name name;
    /// @brief export type
    ExternalType export_type;
    /// @brief external value
    union{
        funcaddr func;
        //TableInst *table;
    } value;
}WpExportInstance;

typedef struct VecExportInstance {
    uint32_t lenght;
    WpExportInstance *elements;
}VecExportInstance;

// Methods **************************************************************************************************

void WpExportInstanceInit(WpExportInstance *self);

#ifdef __cplusplus
    }
#endif

#endif
