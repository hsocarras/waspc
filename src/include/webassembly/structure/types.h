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


#include <stdint.h>

/**
 * @brief The simplest form of value are raw uninterpreted bytes. In the abstract syntax they are represented
 * as hexadecimal literals.
 *      byte ::= 0x00 | . . . | 0xFF
 * 
 */
typedef uint8_t Byte;

typedef struct VecByte{
    uint32_t lenght;
    const Byte *elements;
} VecByte;

/**
 * @brief The types i32 and i64 classify 32 and 64 bit integers, respectively. 
 * Integers are not inherently signed or unsigned,
 * their interpretation is determined by individual operations.
 * 
 */
typedef int32_t I32;
typedef uint32_t U32;
typedef int64_t I64;
typedef uint64_t U64;

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


// Vectors ////////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @brief 
 * 
 */
typedef uint8_t V128[16];

typedef uint8_t VecType[16];
// Vectors ////////////////////////////////////////////////////////////////////////////////////////////////////////////


//Names ///////////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct Name{

    uint32_t lenght;
    const char* name;
}Name;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Reference ///////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef enum RefType{    
    WAS_REFTYPE_NULLREF,
    WAS_REFTYPE_FUNCREF = 0x70,
    WAS_REFTYPE_EXTERNREF = 0x6F,
} RefType;

typedef struct Ref{

    RefType type;

    union {
        RefType null;
        void *func;
        void *external;
    }ref;
}Ref;

typedef struct  VecRef
{
    uint32_t lenght;
    Ref *elements;
}VecRef;


// Reference ///////////////////////////////////////////////////////////////////////////////////////////////////////////

//VALUE Type ///////////////////////////////////////////////////////////////////////////////////////////////////////////
//2.3.4 y 5.3.4
typedef enum ValType {
    

    WAS_FUNCT_TYPE = 0x60,
    WAS_NULL_REF_TYPE,
    WAS_EXTERN_REF_TYPE = 0x6F,
    WAS_FUNC_REF_TYPE = 0x70,
    WAS_V128 = 0x7B,
    WAS_F64 = 0x7C,
    WAS_F32 = 0x7D,
    WAS_I64 = 0x7E,
    WAS_I32 = 0x7F,   

} ValType;

typedef struct VecValType{
    uint32_t lenght;
    ValType *elements;
} VecValType;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Result type ///////////////////////////////////////////////////////////////////////////////////////////////////////////
//2.3.5
typedef struct ResultType{
    uint32_t lenght;
    ValType * val_types;
}ResultType;
//////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Function types classify the signature of functions, mapping a vector of parameters to a vector of results.
 * 
 * functype ::= resulttype → resulttype
 */
typedef struct FuncType{
    ResultType params;
    ResultType results;
} FuncType;
//#################################################################################################################


typedef struct Limits {
    U32 min;
    U32 max;
} Limits;

// Memory //////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef Limits MemType;
// Memory //////////////////////////////////////////////////////////////////////////////////////////////////////////

// Tables //////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct TableType{

    Limits lim;    
    RefType et;

} TableType;
// Tables //////////////////////////////////////////////////////////////////////////////////////////////////////////

// Global //////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef struct GlobalType {
    uint8_t m;                //0-constant, 1-variable
    ValType t;
} GlobalType;
// Global //////////////////////////////////////////////////////////////////////////////////////////////////////////

// External //////////////////////////////////////////////////////////////////////////////////////////////////////////
typedef enum ExternalType{
    WP_EXTERNAL_TYPE_FUNC,
    WP_EXTERNAL_TYPE_TABLE,
    WP_EXTERNAL_TYPE_MEMORY,
    WP_EXTERNAL_TYPE_GLOBAL,    
}ExternalType;

// External //////////////////////////////////////////////////////////////////////////////////////////////////////////

typedef struct Expr{
    const uint8_t *end;
    const uint8_t *instr;
} Expr;

typedef struct VecExpr{
    uint32_t lenght;
    Expr *elements;    
} VecExpr;

#ifdef __cplusplus
    }
#endif

#endif