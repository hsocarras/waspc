#ifndef WASPC_WEBASEMBLY_VALUES_H
#define WASPC_WEBASEMBLY_VALUES_H

#ifdef __cplusplus
    extern "C" {
#endif

#include <stdint.h>

// Define the types for WebAssembly values
typedef enum WasValType {
    WAS_EX_VAL_TYPE_NULL = 0x00,
    WAS_VAL_TYPE_I32 = 0x7F,
    WAS_VAL_TYPE_I64 = 0x7E,
    WAS_VAL_TYPE_F32 = 0x7D,
    WAS_VAL_TYPE_F64 = 0x7C,
    WAS_VAL_TYPE_V128 = 0x7B
} WasValType;

/**
 * @brief The simplest form of value are raw uninterpreted bytes. In the abstract syntax they are represented
 * as hexadecimal literals.
 *      byte ::= 0x00 | . . . | 0xFF
 * 
 */
typedef uint8_t Byte;

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

typedef struct WasValue{
    WasValType type;
    union val{
        I32 i32;
        I64 i64;
        F32 f32;
        F64 f64;
        V128 v128;
    } value;
} WasValue;

#ifdef __cplusplus
    }
#endif

#endif // WASPC_WEBASEMBLY_VALUES_H