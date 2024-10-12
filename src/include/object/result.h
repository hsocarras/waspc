/**
 * @file object.h
 * @author Hector E. Socarras (hsocarras1987@gmail.com)
 * @brief Header file for waspc runtime result object.
 * @version 0.1
 * @date 2024-06-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef WASPC_OBJECT_RESULT_H
#define WASPC_OBJECT_RESULT_H

#ifdef __cplusplus
    extern "C" {
#endif

//wasp includes
#include "object/object.h"
#include "object/error.h"
#include "diagnostic/diag.h"

#include <stdint.h>

/**
 * @brief Enum with all type suported by waspc runtime ecosystem
 * 
 */
typedef enum WpResultType{
    WP_OBJECT_RESULT_TYPE_VALID,
    WP_OBJECT_RESULT_TYPE_ERROR,
}WpResultType;

/**
 * @brief union for all  possible return types;
 * 
 */
typedef union ResultValue{
    WpObjectError err;
    uint32_t u32;
} ResultValue;

/**
 * @brief Result object for return values for most waspc internal functions.
 * 
 */
typedef struct WpObjectResult{
    /// head for all Waspc object to allow cast
    WpObjectType type;
    /// type of result
    WpResultType result_type;
    ///Result value
    ResultValue value;

} WpObjectResult;

// Methods **************************************************************************************************

void ObjectResultInit(WpObjectResult *self);

void ObjectResultAddError (WpObjectResult *self, WpDiagId id, WpDiagModuleList mod);

#ifdef __cplusplus
    }
#endif

#endif