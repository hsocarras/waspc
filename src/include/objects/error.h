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

    /// errors fields
    uint32_t error_type;        //TODO  enum with diferend types of error.
    uint32_t id;                //TODO  unique id for every error, can be used to track the error in logs and diagnostics.
    uint32_t timestamp;     //TODO  timestamp of when the error was generated, can be used to track the error in logs and diagnostics.
    char *msg;          //TODO string message defined in diagnostic module.

    #if WASPC_CONFIG_DEV_FLAG == 1
    uint32_t module_id;
    uint32_t func_code;      //code for the function where the error was generated, unique for every module and function
    uint32_t block_code;       //error code within the function, unique for every block of code where the error can be generated
    uint32_t err_code;
    #endif
    

} WpError;

// Methods **************************************************************************************************


void WpErrorInit(WpError *self);

// **********************************************************************************************************

#ifdef __cplusplus
    }
#endif

#endif