/**
 * @file object.h
 * @author Hector E. Socarras (hsocarras1987@gmail.com)
 * @brief Header file for waspc runtime base object.
 * @version 0.1
 * @date 2024-06-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef WASPC_OBJECT_OBJECT_H
#define WASPC_OBJECT_OBJECT_H

#ifdef __cplusplus
    extern "C" {
#endif

//wasp includes


#include <stdint.h>

/**
 * @brief Enum with all objects defined in waspc runtime
 * 
 */
typedef enum {
    WP_OBJECT_ERROR,
    WP_OBJECT_RESULT,
    WP_OBJECT_WASM_BIN_MODULE,
    WP_OBJECT_WASM_MODULE,
}WpObjectType;

typedef struct WpObject{
    WpObjectType type;
} WpObject;

#ifdef __cplusplus
    }
#endif

#endif