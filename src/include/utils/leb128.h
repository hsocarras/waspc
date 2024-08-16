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

    /// Decoded 32 integer in LEB32 format
    typedef struct {
        uint8_t len;            /// length in byte of encoded integer
        int32_t value;          /// integer value
    } DEC_INT32_LEB128;

    /// Decoded 32 unsigend integer in LEB32 format
    typedef struct {
        uint8_t len;            /// length in byte of encoded integer            
        uint32_t value;          /// integer value
    } DEC_UINT32_LEB128;

    /**
     * @brief Function to decode a 32 bit LEB128 integer      
     * @return DEC_INT32_LEB128 
     */
    DEC_INT32_LEB128 DecodeLeb128Int32(const uint8_t *);

    /**
     * @brief Function to decode a 32 bit unsigned LEB128 integer 
     * 
     * @return DEC_UINT32_LEB128 
     */
    DEC_UINT32_LEB128 DecodeLeb128UInt32(const uint8_t *);

#ifdef __cplusplus
    }
#endif

#endif