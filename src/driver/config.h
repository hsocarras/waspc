/**
 * @file config.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef HOST_WASPC_CONFIG_H
#define HOST_WASPC_CONFIG_H

#ifdef __cplusplus
    extern "C" {
#endif

#include "diagnostic/error.h"

#include <stdint.h>

/**
 * @brief Al hos for waspc runtime must hava a config.h file
 * 
 */

#define ENABLE_DEBUG_INTERPRETER 0

void ReportError(WpError *err);


#ifdef __cplusplus
    }
#endif

#endif