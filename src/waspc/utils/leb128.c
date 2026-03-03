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


//C standard library includes
#include <assert.h>

/**
 * @brief Function to decode a 32 bit LEB128 integer
 * 
 * @param buff pointer to first byte for encoded integer
 * @param n number of bits of the integer to decode (e.g., 32)
 * @param val reference to store decoded value
 * @return const uint8_t* pointer to next byte after encoded integer if success, otherwise is null
 */
const uint8_t * DecodeLeb128Int(const uint8_t *buff, uint8_t n, void *val) {

    assert(n > 8); //n must higher than 8

    uint8_t byte;               /// hold the current procesing byte from encoded array
    uint8_t cicle_counter = 0;  /// nasa golden rule 2
    uint8_t shift = 0;          /// value to shift
    leb128s value;              /// variable to store the decoded int
    uint8_t max_bytes;

    value.i64 = 0;              //initialize value to 0

    if(n % 7 == 0){
        max_bytes = n / 7;
    }
    else{
        max_bytes = (n / 7) + 1;
    }

    while(cicle_counter < max_bytes){   // and encoded int32 must have no more than 5 bytes ceil (N/7)
        //getting the first byte
        byte = *buff;
        //decode
        value.i64 = value.i64 | ((byte & 0x7F) << shift);
        //value to shift in next iteration
        shift = shift + 7;
        //if most significant bit is 0 then byte is the last byte.
        if((byte & 0x80) == 0){
            //check bit for signed value            
            if(shift < 32 && (byte & 0x40) != 0){
                value.i64 = value.i64 | (~0 << shift); 
            }

            buff++;

            
            if(n <= 32){
                *((int32_t *)val) = value.i32;
                return buff;
            }
            else if(n <= 64){
                *((int64_t *)val) = value.i64;
                return buff;
            }
            else{
                return NULL; //invalid n
            }
            
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
 * @param n number of bits of the integer to decode (e.g., 32)
 * @param val reference to store decoded value
 * @return const uint8_t* pointer to next byte after encoded integer if success, otherwise is null
 */
const uint8_t * DecodeLeb128UInt(const uint8_t *buff, uint8_t n, void *val) {

    assert(n > 8); //n must higher than 8

    uint8_t byte;               /// hold the current procesing byte from encoded array
    uint8_t cicle_counter = 0;  /// nasa golden rule 2
    uint8_t shift = 0;          /// value to shift
    leb128u value;              /// variable to store the decoded uint
    uint8_t max_bytes;

    value.u64 = 0;              //initialize value to 0

    if(n % 7 == 0){
        max_bytes = n / 7;
    }
    else{
        max_bytes = (n / 7) + 1;
    }

    while(cicle_counter < max_bytes){  
        //getting the first byte
        byte = *buff;
        //decode
        value.u64 = value.u64 | ((byte & 0x7F) << shift);
        //if most significant bit is 0 then byte is the last byte.
        if((byte & 0x80) == 0){      
            
            
            if(n <= 32){
                *((uint32_t *)val) = value.u32;
                buff++;
                return buff;
            }
            else if(n <= 64){
                *((uint64_t *)val) = value.u64;
                buff++;
                return buff;
            }
            else{
                return NULL; //invalid n
            }                
        }
        //otherwise increment pointer and cicle_counter
        shift = shift + 7;
        buff++;
        cicle_counter++;
    }

    ///the function should not reach this line    
    return NULL;

}