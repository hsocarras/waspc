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
 * @brief Unions to work with different leb128 unsigned types
 */
typedef union leb128u
{
    uint8_t u8;
    uint16_t u16;
    uint32_t u32;
    uint64_t u64;
    
}leb128u;

/**
 * @brief Unions to work with different leb128 signed types
 */
typedef union leb128s
{
    int8_t i8;
    int16_t i16;
    int32_t i32;
    int64_t i64;
    
}leb128s;


    /**
     * Function to decode a 32 bit LEB128 integer      
     * Return a pointer to next uint8_t if success otherwise a null pointer
     */
    const uint8_t * DecodeLeb128Int(const uint8_t *, uint8_t n, void *);

    /**
     * Function to decode a 32 bit unsigned LEB128 integer 
     * Return a pointer to next uint8_t if success otherwise a null pointer      
     */
    const uint8_t * DecodeLeb128UInt(const uint8_t *, uint8_t n, void *);


// Macros for common use cases
#define DecodeLeb128Int32(buff, val) DecodeLeb128Int(buff, 32, (void *) val)
#define DecodeLeb128UInt32(buff, val) DecodeLeb128UInt(buff, 32, (void *) val)
#define DecodeLeb128Int64(buff, val) DecodeLeb128Int(buff, 64, (void *) val)
#define DecodeLeb128UInt64(buff, val) DecodeLeb128UInt(buff, 64, (void *) val)

#ifdef __cplusplus
    }
#endif

#endif