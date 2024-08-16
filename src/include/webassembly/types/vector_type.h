/**
 * @file function_instance.h
 * @author Hector E. Socarras (hsocarras1987@gmail.com)
 * @brief Header file for webassembly function type  related to spec's structure chapter
 * @version 0.1
 * @date 2024-06-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef WASPC_WEBASSEMBLY_STRUCTURE_VECTOR_TYPE_H
#define WASPC_WEBASSEMBLY_STRUCTURE_VECTOR_TYPE_H

#ifdef __cplusplus
    extern "C" {
#endif


#include <stdint.h>

typedef uint8_t V128[8];

typedef V128 VecType;

#ifdef __cplusplus
    }
#endif

#endif