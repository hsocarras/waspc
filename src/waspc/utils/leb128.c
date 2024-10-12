/**
 * @file leb128.c
 * @author Hector E. Socarras (hsocarras1987@gmail.com)
 * @brief File with functions to work with leb128 encode format
 * @version 0.1
 * @date 2024-06-27
 * 
 * @copyright Copyright (c) 2024
 * 
 */

//Project includes
#include "utils/leb128.h"

/**
 * @brief Function to decode a 32 bit LEB128 integer
 * 
 * @param buff pointer to first byte for encoded integer
 * @param ref_i32 reference to store decoded value
 * @return const uint8_t* pointer to next byte after encoded integer if success, otherwise is null
 */
const uint8_t * DecodeLeb128Int32_Fast(const uint8_t *buff, int32_t *ref_i32) {

    uint8_t byte;               /// hold the current procesing byte from encoded array
    uint8_t cicle_counter = 0;  /// nasa golden rule 2
    uint8_t shift = 0;          /// value to shift
    int32_t value = 0;          /// variable to store the decoded int32

    while(cicle_counter < 5){   // and encoded int32 must have no more than 5 bytes ceil (N/7)
        //getting the first byte
        byte = *buff;
        //decode
        value = value | ((byte & 0x7F) << shift);
        //value to shift in next iteration
        shift = shift + 7;
        //if most significant bit is 0 then byte is the last byte.
        if((byte & 0x80) == 0){
            //check bit for signed value            
            if(shift < 32 && (byte & 0x40) != 0){
                *ref_i32 = value | (~0 << shift);
                return buff;
            }
            *ref_i32 = value;
            return buff;
        }
        //otherwise increment pointer and cicle_counter
        buff++;
        cicle_counter++;
    }

    ///the function should not reach this line    
    return NULL;

}

/**
 * @brief Function to decode a 32 bit LEB128 unsigned integer
 * 
 * @param buff pointer to first byte for encoded integer
 * @param ref_u32 reference to store decoded value
 * @return const uint8_t* pointer to next byte after encoded integer if success, otherwise is null
 */
const uint8_t * DecodeLeb128UInt32(const uint8_t *buff, uint32_t *ref_u32) {

    uint8_t byte;               /// hold the current procesing byte from encoded array
    uint8_t cicle_counter = 0;  /// nasa golden rule 2
    uint8_t shift = 0;          /// value to shift
    int32_t value = 0;          /// variable to store the decoded int32

    while(cicle_counter < 5){   // and encoded uint32 must have no more than 5 bytes
        //getting the first byte
        byte = *buff;
        //decode
        value = value | ((byte & 0x7F) << shift);
        //if most significant bit is 0 then byte is the last byte.
        if((byte & 0x80) == 0){      
            
            *ref_u32 = value;     
            return buff;
        }
        //otherwise increment pointer and cicle_counter
        shift = shift + 7;
        buff++;
        cicle_counter++;
    }

    ///the function should not reach this line    
    return NULL;

}