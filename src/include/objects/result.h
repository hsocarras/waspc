/**
 * @file result.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-02-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#ifndef WASPC_OBJECTS_RESULT_H
#define WASPC_OBJECTS_RESULT_H

#ifdef __cplusplus
    extern "C" {
#endif

//wasp includes
#include "objects/object.h"
#include "interpreter/values.h"

#include <stdint.h>

typedef struct WpResult {
    /// head for all Waspc object to allow cast
    WpObjectType wp_type;

    StackValue *vals;
    uint32_t len;

}WpResult;


// Methods **************************************************************************************************

void WpResultInit(WpResult *self);

#ifdef __cplusplus
    }
#endif

#endif
