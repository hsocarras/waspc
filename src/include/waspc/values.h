/**
 * @file values.h
 * @author Hector E. Socarras (hsocarras1987@gmail.com)
 * @brief 
 * @version 0.1
 * @date 2024-07-08
 * 
 * @copyright Copyright (c) 2024
 * 
 */
 

#ifndef AOS_VM_VALUES_H
#define AOS_VM_VALUES_H

#ifdef __cplusplus
    extern "C" {
#endif

#include <stdint.h>

/**
 * @brief Function instances, table instances, memory instances, and global instances, element instances, and data instances
 * in the store are referenced with abstract addresses. These are simply indices into the respective store component.
 * In addition, an embedder may supply an uninterpreted set of host addresses.
 * 
 */
 typedef uint32_t addr;

/**
 * @brief 
 * 
 */
 typedef enum{
    func,
    table,
    mem,
    global,
 }ExternValType;

/**
 * @brief 
 * 
 */
typedef struct{
    ExternValType type;
    addr address;
}ExternVal;

/**
 * @brief enum to define diferent datatatype suported by VM.
 * 
 */
typedef enum {
    NONE,           /// Stand for internal null value
    I32,            /// Stand for wsam signed 32 bits integer
    I64,            /// Stand for wsam signed 64 bits integer
    U32,            /// Stand for wsam unsigned 32 bits integer
    U64,            /// Stand for wsam unsigned 64 bits integer
    F32,            /// Stand for wsam 32 bits floating number
    F64,            /// Stand for wsam 64 bits floating number
    V128,           /// Stand for vector
} DataType;

/**
 * @brief Structure to define item for value stack
 * 
 */
typedef struct {
    DataType type;  /// type of data
    union {    
        uint32_t u32;
        int32_t i32;
        uint64_t u64;
        int64_t i64;
        float f32;
        double f64;
    } as;           ///content of stack
 }Value;

/**
 * @brief Type for 
 * 
 */
 typedef struct {
    Value *ret;         /// ok
 }Result;

///NULL Value
static const Value null_value = {.DataType=NONE, as={.u32=0}};

#ifdef __cplusplus
    }
#endif

#endif