/**
 * @file wasdef.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-07-13
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef AOS_VM_WASDEF_H
#define AOS_VM_WASDEF_H

#ifdef __cplusplus
    extern "C" {
#endif

typedef enum {
    CUSTOM,
    TYPE,
    IMPORT,
    FUNCTION,
    TABLE,
    MEMORY,
    GLOBAL,
    EXPORT,
    START,
    ELEMENT,
    CODE,
    DATA,
    DATA_COUNT,       
    
}SectionId;

typedef enum {
    INDEX,
    I32 = 0x7F,
    I64 = 0x7E,

}WasTypes

#ifdef __cplusplus
    }
#endif

#endif