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

#ifndef WASPC_WEBASSEMBLY_STRUCTURE_NUMBER_TYPE_H
#define WASPC_WEBASSEMBLY_STRUCTURE_NUMBER_TYPE_H

#ifdef __cplusplus
    extern "C" {
#endif


#include <stdint.h>

/**
 * @brief i32 datatype acording to web assembly spec
 * 
 */
typedef union I32 {
    uint32_t u;
    int32_t s;
} I32;

/**
 * @brief i64 datatype acording to web assembly spec
 * 
 */
typedef union I64 {
    uint64_t u;
    int64_t s;
} I64;

typedef float F32;

typedef double F64;

/**
 * @brief Number types classify numeric values.
 *
 * numtype ::= i32 | i64 | f32 | f64
 * 
 */
typedef union NumType{
    I32 i32;
    I64 i64;
    F32 f32;
    F64 f64;
} NumType;

#ifdef __cplusplus
    }
#endif

#endif