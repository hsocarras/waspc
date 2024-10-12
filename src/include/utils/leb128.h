/**
 * @file leb128.h
 * @author Hector E. Socarras (hsocarras1987@gmail.com)
 * @brief Header file for utility function for workint with LEB128 format
 * @version 0.1
 * @date 2024-06-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef WASPC_UTILS_LEB128_H
#define WASPC_UTILS_LEB128_H

#ifdef __cplusplus
    extern "C" {
#endif

#include <stdint.h>
    
    /**
     * Function to decode a 32 bit LEB128 integer      
     * Return a pointer to next uint8_t if success otherwise a null pointer
     */
    const uint8_t * DecodeLeb128Int32(const uint8_t *, int32_t *);

    /**
     * Function to decode a 32 bit unsigned LEB128 integer 
     * Return a pointer to next uint8_t if success otherwise a null pointer      
     */
    const uint8_t * DecodeLeb128UInt32(const uint8_t *, uint32_t *);

#ifdef __cplusplus
    }
#endif

#endif