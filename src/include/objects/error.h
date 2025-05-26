/**
 * @file object.h
 * @author Hector E. Socarras (hsocarras1987@gmail.com)
 * @brief Header file for waspc error object.
 * @version 0.1
 * @date 2024-06-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef WASPC_OBJECT_ERROR_H
#define WASPC_OBJECT_ERROR_H

#ifdef __cplusplus
    extern "C" {
#endif

//wasp includes
#include "objects/object.h"

#include <stdint.h>

/**
 * @brief Error object for return values for most waspc internal functions.
 * 
 */
typedef struct WpError{

    /// head for all Waspc object to allow cast
    WpObjectType type;

    /// errors id, unique for every posible error
    uint32_t id;     

    #if WASPC_CONFIG_DEV_FLAG == 1
    char file[64];
    char func[32];
    #endif

} WpError;

// Methods **************************************************************************************************


void WpErrorInit(WpError *self);

// **********************************************************************************************************

#ifdef __cplusplus
    }
#endif

#endif