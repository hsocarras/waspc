/**
 * @file hash_table.h
 * @author Hector E. Socarras (hsocarras1987@gmail.com)
 * @brief  Header file with deficition for frame. That is main interpreter executable block.
 * @version 0.1
 * @date 2024-06-25
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef WASPC_UTILS_HASH_TABLE_H
#define WASPC_UTILS_HASH_TABLE_H

#ifdef __cplusplus
    extern "C" {
#endif
#include "objects/module_state.h"

#include <stdint.h>
#include <string.h>

uint32_t fnv(const char *key, size_t len);



#ifdef __cplusplus
    }
#endif

#endif