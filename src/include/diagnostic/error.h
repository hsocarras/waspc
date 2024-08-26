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

#include "object/object.h"

#include <stdint.h>



typedef enum {

    WP_DIAG_ID_OK,
    WP_DIAG_ID_INVALID_MODULE_SIZE,
    WP_DIAG_ID_INVALID_MAGIC,
    WP_DIAG_ID_INVALID_VERSION,
    WP_DIAG_ID_INVALID_SECTION_ID,
    WP_DIAG_ID_TODO,
} WpErrorId;

typedef enum WpErrorModuleList {
    W_DIAG_MOD_LIST_HOST,
    W_DIAG_MOD_LIST_LOADER,
    W_DIAG_MOD_LIST_DECODER,
    W_DIAG_MOD_LIST_UTILS,
} WpErrorModuleList;

typedef struct WpErrorModuleInfo {

    WpErrorModuleList origin;               // module
    uint32_t func;                          // function inside module
    uint32_t place;                         // check wher the error was produced
    
} WpErrorModuleInfo;

typedef struct {

    WpObject head;          /// header
    
    WpErrorId id;           /// diagnostic id for text and severity

    WpErrorModuleInfo detail;       //TODO make a condifcional for debug flag
    
} WpError;

WpError CreateError(WpErrorId, WpErrorModuleList mod, uint32_t func, uint32_t place);

extern void ReportError(WpError *err);

#ifdef __cplusplus
    }
#endif

#endif