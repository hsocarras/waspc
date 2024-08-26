/**
 * @file values.h
 * @author Hector E. Socarras (hsocarras1987@gmail.com)
 * @brief Header file for webassembly values related to spec's runtime chapter
 * @version 0.1
 * @date 2024-06-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef WASPC_WEBASSEMBLY_RUNTIME_ADDRESSES_H
#define WASPC_WEBASSEMBLY_RUNTIME_ADDRESSES_H

#ifdef __cplusplus
    extern "C" {
#endif

/**
 * @brief Function instances, table instances, memory instances, and global instances, element instances, and data instances
 * in the store are referenced with abstract addresses.
 * addr ::= 0 | 1 | 2 | . . .
 */
typedef void *addr;




#ifdef __cplusplus
    }
#endif

#endif