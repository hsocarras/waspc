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
 * @brief Function to decode an array of byte into a 32 bits integer.
 *  TODO add a parameter with pointer to buff end.
 * @param buff Pointer to first buffer's bytes with encoded 32 bit integer.
 * @return DEC_INT32_LEB128 struct.
 */
DEC_INT32_LEB128 DecodeLeb128Int32(const uint8_t *buff){

    uint8_t byte;               /// hold the current procesing byte from encoded array
    uint8_t cicle_counter = 0;  /// nasa golden rule 2
    uint8_t shift = 0;          /// value to shift
    int32_t value = 0;          /// variable to store the decoded int32

    DEC_INT32_LEB128 result = {0, 0};         

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
            result.len = cicle_counter + 1; //Number of encoded bytes
            if(shift < 32 && (byte & 0x40) != 0){
                result.value = value | (~0 << shift);
                return result;
            }
            result.value = value;
            return result;
        }
        //otherwise increment pointer and cicle_counter
        buff++;
        cicle_counter++;
    }
    ///the program should not reach this line
    result.len = 0;    
    return result; 
}

/**
 * @brief Function to decode an array of byte into a 32 bits unsigned integer.
 *
 * @param buff Pointer to first buffer's bytes with encoded 32 bit unsigned integer.
 * @return DEC_UINT32_LEB128 struct.
 */
DEC_UINT32_LEB128 DecodeLeb128UInt32(const uint8_t *buff){

    uint8_t byte;               /// hold the current procesing byte from encoded array
    uint8_t cicle_counter = 0;  /// nasa golden rule 2
    uint8_t shift = 0;          /// value to shift
    uint32_t value = 0;         /// variable to store the decoded uint32

    DEC_UINT32_LEB128 result = {0, 0}; 

    while(cicle_counter < 5){   // and encoded uint32 must have no more than 5 bytes
        //getting the first byte
        byte = *buff;
        //decode
        value = value | ((byte & 0x7F) << shift);
        //if most significant bit is 0 then byte is the last byte.
        if((byte & 0x80) == 0){       
            result.len = cicle_counter + 1; //Number of encoded bytes
            result.value = value;     
            return result;
        }
        //otherwise increment pointer and cicle_counter
        shift = shift + 7;
        buff++;
        cicle_counter++;
    }
    ///the program should not reach this line
    result.len = 0;
    return result; 
}