/**
 * @file frame.h
 * @author Hector E. Socarras (hsocarras1987@gmail.com)
 * @brief  Header file with deficition for frame. That is main interpreter executable block.
 * @version 0.1
 * @date 2024-06-25
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef AOS_INTERNAL_FRAME_H
#define AOS_INTERNAL_FRAME_H

#ifdef __cplusplus
    extern "C" {
#endif

//Project include
#include "vm/values.h"
//Standard include
#include <stdint.h>

    /// Code block
    typedef struct {
        uint8_t* code;          /// pointer where the webassembly expresions start inside code section
        uint32_t code_len;      /// length of segment with expresion
        uint8_t *ip;            /// instruction pointer to save the return address after a function call
        Value * local;          /// pointer to vm's value stack where local variables for this frame start
    } Frame;

    static const Frame null_frame = {null, 0, null, null};

    //Implemented in frame.c
    Frame CreateNewFrame(uint8_t *, uint32_t);

#ifdef __cplusplus
    }
#endif

#endif