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
    WpObjectType wp_type;

    /// errors id, unique for every posible error
    uint32_t id;   
    
    //TODO : this mus be only for dev
    uint8_t module_id;
    uint8_t code; //code for the error, unique for every module
    uint8_t subcode; //subcode for the error, unique for every module and code
    uint8_t severity; //severity of the error, from 0 to 5, where 0 is the least severe and 5 is the most severe

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