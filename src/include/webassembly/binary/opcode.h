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
    typedef enum {
        OPCODE_UNREACHABLE = 0x00,
        OPCODE_NOP = 0x01,                  /// No operation
        
        OPCODE_END = 0X0B,
        OPCODE_RETURN = 0x0F,               /// return instruction
        OPCODE_LOCAL_GET = 0x20,            /// get local variable x
        OPCODE_LOCAL_SET = 0x21,            /// set local variable x
        OPCODE_LOCAL_TEE = 0x22,
        OPCODE_GLOBAL_GET = 0x23,
        OPCODE_GLOBAL_SET = 0x24,
        OPCODE_I32_LOAD = 0x28,
        OPCODE_I64_LOAD = 0x29,
        OPCODE_F32_LOAD = 0x2A,
        OPCODE_F64_LOAD = 0x2B,
        OPCODE_I32_LOAD_I8 = 0x2C,

        OPCODE_I32_CONST = 0x41,            /// load const I32 type to stack
        OPCODE_I64_CONST = 0x42,            /// load const I64 type to stack
        OPCODE_F32_CONST = 0x43,            /// load const F32 type to stack
        OPCODE_F64_CONST = 0x44,            /// load const F64 type to stack

        OPCODE_I32_EQZ = 0x45,
        OPCODE_I32_EQ = 0x46,
        OPCODE_I32_NE = 0x47,

        OPCODE_I32_SUB = 0x6B,

        OPCODE_I32_XOR = 0X73,
    }OpCode;

#ifdef __cplusplus
    }
#endif

#endif