/**
 * @file types.h
 * @author Hector E. Socarras (hsocarras1987@gmail.com)
 * @brief Header file for webassembly values related to spec's runtime chapter
 * @version 0.1
 * @date 2024-06-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef WASPC_WEBASSEMBLY_STRUCTURE_TYPES_H
#define WASPC_WEBASSEMBLY_STRUCTURE_TYPES_H

#ifdef __cplusplus
    extern "C" {
#endif

#include "webassembly/runtime/addresses.h"

#include <stdint.h>

/**
 * @brief The simplest form of value are raw uninterpreted bytes. In the abstract syntax they are represented
 * as hexadecimal literals.
 *      byte ::= 0x00 | . . . | 0xFF
 * 
 */
typedef uint8_t byte;

/**
 * @brief The types i32 and i64 classify 32 and 64 bit integers, respectively. 
 * Integers are not inherently signed or unsigned,
 * their interpretation is determined by individual operations.
 * 
 */
typedef union I32{
    int32_t s;
    uint32_t u;
} I32;

/**
 * @brief The types i32 and i64 classify 32 and 64 bit integers, respectively. 
 * Integers are not inherently signed or unsigned,
 * their interpretation is determined by individual operations.
 * 
 */
typedef union I64{
    int64_t s;
    uint64_t u;
} I64;

/**
 * @brief The types f32 and f64 classify 32 and 64 bit floating-point data, respectively. They correspond to the respective
 * binary floating-point representations, also known as single and double precision, as defined by the IEEE 75412
 * standard (Section 3.3).
 * 
 */
typedef float F32;

/**
 * @brief The types f32 and f64 classify 32 and 64 bit floating-point data, respectively. They correspond to the respective
 * binary floating-point representations, also known as single and double precision, as defined by the IEEE 75412
 * standard (Section 3.3).
 * 
 */
typedef double F64;

/**
 * @brief Number types classify numeric values.
 *      numtype ::= i32 | i64 | f32 | f64
 * 
 */
typedef union NumType {
    I32 i32;
    I64 i64;
    F32 f32;
    F64 f64;
} NumType;

// Vectors ////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief 
 * 
 */
typedef uint8_t V128[16];

typedef uint8_t VecType[16];
// Vectors ////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Reference ///////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef void *FuncRef;

typedef void *ExternRef;

typedef union RefType {
    FuncRef funcref;
    ExternRef externref; 
} RefType;
// Reference ///////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Value types classify the individual values that WebAssembly code can compute with and the values that a variable
 * accepts. They are either number types, vector types, or reference types.
 *  valtype ::= numtype | vectype | reftype
 * 
 */
typedef union ValType {
    NumType num;
    VecType v128;
    RefType ref;
} Valtype;

/**
 * @brief Result types classify the result of executing instructions or functions, which is a sequence of values, written with
 * brackets.
 *      resulttype ::= [vec(valtype)]
 * 
 */
typedef struct ResultType {
    uint32_t count;
    ValType *vec;
}ResultType;


typedef struct FuncType{
    ResultType param;
    ResultType ret;
} FuncType;

typedef struct Limits {
    uint32_t min;
    uint32_t max;
} Limits;

// Memory //////////////////////////////////////////////////////////////////////////////////////////////////////////
//typedef Limits MemType;
// Memory //////////////////////////////////////////////////////////////////////////////////////////////////////////

// Tables //////////////////////////////////////////////////////////////////////////////////////////////////////////
//typedef Limits MemType;
// Tables //////////////////////////////////////////////////////////////////////////////////////////////////////////

// Global //////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum Mut{
    CONST,
    VAR,
} Mut;

typedef struct GlobalType {
    Mut mut;
    ValType value;
} GlobalType;
// Global //////////////////////////////////////////////////////////////////////////////////////////////////////////

// External //////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef union ExternType {
    FuncType func;
    TableType table;
    MemType mem;
    GlobalType global;
} ExternType;
// External //////////////////////////////////////////////////////////////////////////////////////////////////////////


#ifdef __cplusplus
    }
#endif

#endif