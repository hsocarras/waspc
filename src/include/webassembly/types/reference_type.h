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

#ifndef WASPC_WEBASSEMBLY_STRUCTURE_REFERENCE_TYPE_H
#define WASPC_WEBASSEMBLY_STRUCTURE_REFERENCE_TYPE_H

#ifdef __cplusplus
    extern "C" {
#endif


#include <stdint.h>

typedef struct FuncRef{
    uint8_t dummy
} FuncRef;

typedef struct ExternRef{
    uint8_t dummy
} ExternRef;

typedef union RefType{
    FuncRef funcref;
    ExternRef externref;
} RefType;

#ifdef __cplusplus
    }
#endif

#endif