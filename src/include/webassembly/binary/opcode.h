/**
 * @file opcode.h
 * @author H. E.Socarras
 * @brief Webassembly opcodes definition
 * @version 0.1
 * @date 2024-06-25
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef WASPC_OPCODE_H
#define WASPC_OPCODE_H

#ifdef __cplusplus
    extern "C" {
#endif

    /// Enum with webassembly op codes
    typedef enum : unsigned short{
        OP_UNREACHABLE = 0x00,
        OP_NOP = 0x01,                  /// No operation
        
        OP_RETURN = 0x0F,               /// return instruction
        OP_LOCAL_GET = 0x20,            /// get local variable x
        OP_LOCAL_SET = 0x21,            /// set local variable x
        OP_LOCAL_TEE = 0x22,
        OP_GLOBAL_GET = 0x23,
        OP_GLOBAL_SET = 0x24,
        OP_I32_LOAD = 0x28,
        OP_I64_LOAD = 0x29,
        OP_F32_LOAD = 0x2A,
        OP_F64_LOAD = 0x2B,
        OP_I32_LOAD_I8 = 0x2C,

        OP_I32_CONST = 0x41,            /// load const I32 type to stack
        OP_I64_CONST = 0x42,            /// load const I64 type to stack
        OP_F32_CONST = 0x43,            /// load const F32 type to stack
        OP_F64_CONST = 0x44,            /// load const F64 type to stack

        OP_I32_EQZ = 0x45,
        OP_I32_EQ = 0x46,
        OP_I32_NE = 0x47,

        OP_I32_SUB = 0x6B,

        OP_I32_XOR = 0X73,
    }OpCode;

#ifdef __cplusplus
    }
#endif

#endif