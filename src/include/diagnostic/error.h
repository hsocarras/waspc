/**
 * @file error.h
 * @author Hector E. Socarras (hsocarras1987@gmail.com)
 * @brief Header file for webassembly virtual machine
 * @version 0.1
 * @date 2024-06-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef WASPC_DIAGNOSTIC_ERROR_H
#define WASPC_DIAGNOSTIC_ERROR_H

#ifdef __cplusplus
    extern "C" {
#endif

#include <stdint.h>

typedef enum {

    OK,
    INVALID_MODULE_SIZE,
    INVALID_MAGIC,
    INVALID_VERSION,
    INVALID_SECTION_ID,
} WpErrorId;

typedef struct {
    
    /// diagnostic id for text and severity
    WpErrorId id;       

} WpError;

#ifdef __cplusplus
    }
#endif

#endif