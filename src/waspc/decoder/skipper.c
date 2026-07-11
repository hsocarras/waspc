/**
 * @file wasm_instanciate.c
 * @author your name (you@domain.com)
 * @version 0.1
 * @date 2024-08-21
 *
 * @brief This file contains the implementation of the functions to decode binary WebAssembly module.
 * The functions in this file are in three kinds:
 *  - functions to skip a binary encoded non terminal symbol
 *  - functions to get the start of a binary encoded non terminal symbol.
 *  - functions to decode a binary encoded non terminal symbol.
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "decoder/wasm_decoder.h"
#include "decoder/leb128.h"
#include "webassembly/instructions.h"

#include <assert.h>

/**
 * @version 3.0
 * @brief  Function to decode expr rule
 * expr ::= (in:instr)* 0x0B ⇒ in* end
 *
 * @param buf segment for a binary web asembly module
 * @param max_len max len to traverse the buffer
 * @return const uint8_t* pointer to next byte after expr
 * @example
 *  uint8_t binary_module[] = {0x41, 0x01, 0x0B, 0x20, 0x00, 0x0B}; // Example binary module segment
    const uint8_t *result = SkipExprBuf(binary_module, end_binary_module);
    if (result) {
        printf("Next byte after expr: 0x%02X\n", *result);
    } else {
        printf("Failed to decode expr or reached max length without finding END.\n");
    }
 */
const uint8_t *SkipExprBuf(const uint8_t *const buf, const uint8_t *const buf_end)
{
    #if WASPC_CONFIG_DEV_FLAG == 1    
    assert(buf != NULL); // Check if typesec content is not NULL
    #endif

    const uint8_t *index = buf;
    uint8_t byte_val;

#define READ_BYTE() (*index++)
#define NOT_END() (index < buf_end)

    // look for opcode 0x0B;
    byte_val = READ_BYTE();
    while (NOT_END() && byte_val != OPCODE_END)
    {
        byte_val = READ_BYTE();
    }
    //IF OPCODE_END Found return ok
    if (*(index - 1) == OPCODE_END)
    {
        return index;
    }
    else
    {
        return NULL;
    }

#undef READ_BYTE
#undef NOT_END
}

/**
 * @version v3.0
 * @brief Function to skip a recursive type
 * @param buf Pointer to the start of the recursive type
 * @return const uint8_t* Pointer to the byte after the recursive type, or NULL if an error occurs
 * @example
 *   uint8_t binary_data[] = {0x80, 0x01, 0x01, 0x7F}; // Example binary data
    const uint8_t *result = SkipSubType(binary_data);
    if (result) {
        printf("Next byte after subtype: 0x%02X\n", *result);
    } else {
        printf("Failed to decode subtype.\n");
    }
 */
const uint8_t *SkipRecSubType(const uint8_t *const buf)
{

    #if WASPC_CONFIG_DEV_FLAG == 1    
    assert(buf != NULL); // Check if typesec content is not NULL
    #endif

    const uint8_t *index = buf;
    uint32_t len;
    uint32_t idx;
    uint8_t byte_val;

#define READ_BYTE() (*index++)
    byte_val = READ_BYTE();

    switch (byte_val)
    {
    case 0x4F:
    case 0x50:
        // 0x4F 𝑥*:list(typeidx) ct:comptypee ⇒ (rec 𝑥* ct)
        index = DecodeLeb128UInt32(index, &len);
        if (!index)
        {
            return NULL;
        }
        for (uint32_t i = 0; i < len; i++)
        {
            index = DecodeLeb128UInt32(index, &idx);
            if (!index)
            {
                return NULL;
            }
        }
        byte_val = READ_BYTE();
        break;
    case 0x5E:                  // 0x5E ft:fieldtype
        byte_val = READ_BYTE(); // get fieldtype
        byte_val = READ_BYTE(); // get mutability
        if (byte_val != 0 && byte_val != 1)
        {
            index--; // mutability is optional now
        }
        break;
    case 0x5F: // 0x5F ft*:list(fieldtype)
        index = DecodeLeb128UInt32(index, &len);
        if (!index)
        {
            return NULL;
        }
        for (uint32_t i = 0; i < len; i++)
        {
            byte_val = READ_BYTE(); // get fieldtype
            byte_val = READ_BYTE(); // get mutability
            if (byte_val != 0 && byte_val != 1)
            {
                index--; // mutability is optional now
            }
        }
        break;
    case 0x60:                                   // 0x60 𝑡* 1:resulttype 𝑡* 2:resulttype
        index = DecodeLeb128UInt32(index, &len); // get param count
        if (!index)
        {
            return NULL;
        }
        index += len;                            // skip param types
        index = DecodeLeb128UInt32(index, &len); // get result count
        if (!index)
        {
            return NULL;
        }
        index += len; // skip result types
        break;
    default:
        return NULL;
        break;
    }
#undef READ_BYTE
    return index;
}

/**
 * @version 3.0
 * @brief Skip limit type in binary wasm module
 * @param buf Pointer to the start of the limit type
 * @return const uint8_t* Pointer to the byte after the limit type, or NULL if an error occurs
 */
const uint8_t *SkipLimitTypeBuf(const uint8_t *const buf)
{
    #if WASPC_CONFIG_DEV_FLAG == 1    
    assert(buf != NULL); // Check if typesec content is not NULL
    #endif

    const uint8_t *index = buf;
    uint64_t aux;

    switch (*index)
    {
    case 0: // 0x00 𝑛:u64 ⇒ (i32, [𝑛 .. ])
        index++;
        index = DecodeLeb128UInt64(index, &aux);
        if (!index)
        {
            return NULL;
        }
        if (aux > 0xFFFFFFFF)
        {
            return NULL;
        }
        break;
    case 1: // 0x01 𝑛:u64 𝑚:u64 ⇒ (i32, [𝑛 .. 𝑚])
        index++;
        index = DecodeLeb128UInt64(index, &aux);
        if (!index)
        {
            return NULL;
        }
        if (aux > 0xFFFFFFFF)
        {
            return NULL;
        }
        index = DecodeLeb128UInt64(index, &aux);
        if (!index)
        {
            return NULL;
        }
        if (aux > 0xFFFFFFFF)
        {
            return NULL;
        }
        break;
    case 4: // 0x04 𝑛:u64 ⇒ (i64, [𝑛 .. ])
        index++;
        index = DecodeLeb128UInt64(index, &aux);
        if (!index)
        {
            return NULL;
        }
        break;
    case 5: // 0x05 𝑛:u64 𝑚:u64 ⇒ (i64, [𝑛 .. 𝑚])
        index = DecodeLeb128UInt64(index, &aux);
        if (!index)
        {
            return NULL;
        }

        index = DecodeLeb128UInt64(index, &aux);
        if (!index)
        {
            return NULL;
        }
        break;
    default:
        return NULL;
        break;
    }

    return index;
}

/**
 * @version v3.0
 * @brief function to skip a reference type inside binary wasm module
 * @param buf Pointer to the start of the reference type
 * @return const uint8_t* Pointer to the byte after the reference type, or NULL if an error occurs
 */
const uint8_t *SkipRefTypeBuf(const uint8_t *const buf)
{   
    #if WASPC_CONFIG_DEV_FLAG == 1    
    assert(buf != NULL); // Check if typesec content is not NULL
    #endif

    const uint8_t *index = buf;
    uint64_t aux;
    // reftype
    switch (*index)
    {
    case 0x63:
    case 0x64:
        index++;
        if (*index >= 0x69 && *index <= 0x74)
        {
            // is a absheadtype
            index++;
        }
        else
        {
            index = DecodeLeb128Int64(index, &aux);
            if (!index)
            {
                return NULL;
            }
        }
        break;
    case 0x69:
    case 0x6A:
    case 0x6B:
    case 0x6C:
    case 0x6D:
    case 0x6E:
    case 0x70:
    case 0x71:
    case 0x72:
    case 0x73:
    case 0x74:
    case 0x75:
        index++;
        break;
    default:
        return NULL;
        break;
    }

    return index;
}

/**
 * @version v3.0
 * @brief function to skip a table type inside table section
 * @param buf Pointer to the start of the table type
 * @return const uint8_t* Pointer to the byte after the table type, or NULL if an error occurs
 */
const uint8_t *SkipTableTypeBuf(const uint8_t *const buf)
{   
    #if WASPC_CONFIG_DEV_FLAG == 1    
    assert(buf != NULL); // Check if typesec content is not NULL
    #endif
    const uint8_t *index = buf;
    uint32_t aux;

    // skip reftype
    index = SkipRefTypeBuf(index);
    if (!index)
    {
        return NULL;
    }
    // skip limit type
    index = SkipLimitTypeBuf(index);
    return index;
}

/**
 * @version v3.0
 * @brief function to skip local buffer is code section.
 * @param buf Pointer to the start of local.
 * @return const uint8_t * Pointer to the byte after local, or NULL if a error occurs.
 */
const uint8_t *SkipLocalBuf(const uint8_t *const buf)
{   
    #if WASPC_CONFIG_DEV_FLAG == 1    
    assert(buf != NULL); // Check if typesec content is not NULL
    #endif

    const uint8_t *index = buf;
    uint32_t local_count, aux;
    // get number of element n
    index = DecodeLeb128UInt32(index, &local_count);
    if (!index)
    {
        return NULL;
    }
    // skip local types
    for (uint32_t i = 0; i < local_count; i++)
    {
        index = DecodeLeb128UInt32(index, &aux);
        if (!index)
        {
            return NULL;
        }
        // skip value type
        index++;
    }
    return index;
}

/**
 * @version v3.0
 * @brief function to skip value type buffer.
 * valtype ::= nt:numtype ⇒ nt
 *           | vt:vectype ⇒ vt
 *           | rt:reftype ⇒ rt
 * @param buf Pointer to the start of value type.
 * @return const uint8_t * Pointer to the byte after local, or NULL if a error occurs.
 */
const uint8_t *SkipValTypeBuf(const uint8_t *const buf)
{   
    #if WASPC_CONFIG_DEV_FLAG == 1    
    assert(buf != NULL); // Check if typesec content is not NULL
    #endif

    const uint8_t *index = buf;
    int64_t s33;
    uint8_t byte_val;

#define READ_BYTE() (*index++)

    if (IsNumericType(index))
    {
        index++;
        return index;
    }

    if (*index == 0x7B)
    {
        index++;
        return index;
    }

    switch (*index)
    {
    case 0x63: // ref null headtype
    case 0x64: // ref head type
        index++;
        if (IsAbsHeadType(index))
        {
            index++;
            return index;
        }
        index = DecodeLeb128Int64(index, &s33);
        if (!index)
        {
            return NULL;
        }
        return index;
        break;
    default:
        if (IsAbsHeadType(index))
        {
            index++;
            return index;
        }
        return NULL;
        break;
    }

#undef READ_BYTE
}

/**
 * @version v3.0
 * @brief function to skip mem argument in memory instruction.
 * memarg ::= 𝑛:u32 𝑚:u32
 *          | 𝑛:u32 𝑥:memidx 𝑚:u32
 * @param buf Pointer to the start of mem arg.
 * @return const uint8_t * Pointer to the byte after mem arg, or NULL if a error occurs.
 */
const uint8_t *SkipMemArgBuf(const uint8_t *const buf)
{   
    #if WASPC_CONFIG_DEV_FLAG == 1    
    assert(buf != NULL); // Check if typesec content is not NULL
    #endif

    const uint8_t *index = buf;
    uint32_t aux;

    index = DecodeLeb128UInt32(index, &aux); // read align
    if (!index)
    {
        return NULL;
    }
    if((aux & 0x08) == 0x08){ // if the bit 6 is set, then there is an mem index field
        index = DecodeLeb128UInt32(index, &aux);
        if (!index)
        {
            return NULL;
        }
        index = DecodeLeb128UInt32(index, &aux);
        if (!index)
        {
            return NULL;
        }
    } else {
        index = DecodeLeb128UInt32(index, &aux);
        if (!index)
        {
            return NULL;
        }
    }
    return index;
}