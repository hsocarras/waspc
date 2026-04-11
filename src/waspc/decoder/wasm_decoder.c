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
#include "utils/leb128.h"
#include "webassembly/instructions.h"

#include <assert.h>

//////////Functions to fast traverse binary //////////////////////////////////////////////////////////////////////////////////////////////////

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
    const uint8_t *result = SkipExprBuf(binary_module, sizeof(binary_module));
    if (result) {
        printf("Next byte after expr: 0x%02X\n", *result);
    } else {
        printf("Failed to decode expr or reached max length without finding END.\n");
    }
 */
const uint8_t *SkipExprBuf(const uint8_t *const buf, const uint32_t max_len)
{

    const uint8_t *index = buf;
    const uint8_t *buf_end = buf + max_len;
    uint8_t byte_val;

#define READ_BYTE() (*index++)
#define NOT_END() (index < buf_end)

    // look for opcode 0x0B;
    byte_val = READ_BYTE();
    while (NOT_END() && byte_val != OPCODE_END)
    {
        byte_val = READ_BYTE();
    }

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

    assert(buf != NULL);

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
 * @return const uint8_t * Pointer to the byte aftel local, or NULL if a error occurs.
 */
const uint8_t *SkipLocalBuf(const uint8_t *const buf)
{
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

const uint8_t *SkipValTypeBuf(const uint8_t *const buf)
{
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

const uint8_t *SkipMemArgBuf(const uint8_t *const buf)
{
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
////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

// Utilitary functions /////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @version 3.0
 * @brief Function to  check if valtype is an abstract head type.
 * @param val_type Pointer to byte with encodet type
 * @return uint8_t 0-non valid, 1-valid.
 */
uint8_t IsAbsHeadType(const uint8_t *valtype)
{

    switch (*valtype)
    {
    case 0x69: // exn
    case 0x6A: // array
    case 0x6B: // struct
    case 0x6C: // i31
    case 0x6D: // eq
    case 0x6E: // any
    case 0x6F: // extern
    case 0x70: // func
    case 0x71: // none
    case 0x72: // noextern
    case 0x73: // nofunc
    case 0x74: // noexn
        return 1;
        break;
    default:
        return 0;
        break;
    }
}

uint8_t IsNumericType(const uint8_t *valtype)
{

    switch (*valtype)
    {
    case 0x7C: // f64
    case 0x7D: // f32
    case 0x7E: // i64
    case 0x7F: // i32
        return 1;
        break;
    default:
        return 0;
        break;
    }
}

uint8_t IsRefType(const uint8_t *valtype)
{

    const uint8_t *index = valtype;
    int64_t s33;

    switch (*valtype)
    {
    case 0x63: // ref null headtype
    case 0x64: // ref head type
        index++;
        if (IsAbsHeadType(index))
        {
            return 1;
        }
        index = DecodeLeb128Int64(index, &s33);
        if (!index)
        {
            return 0;
        }
        return 1;
        break;
    default:
        if (IsAbsHeadType(index))
        {
            return 1;
        }
        return 0;
        break;
    }
}

/**
 * @version 3.0
 * @brief Function to heck if valtype is a value type.
 * @param val_type
 * @return uint8_t 0-non valid, 1-valid.
 */
uint8_t IsValType(const uint8_t *valtype)
{

    if (IsNumericType(valtype))
    {
        return 1;
    }

    if (*valtype == 0x7B)
    {
        return 1;
    }

    if (IsRefType(valtype))
    {
        return 1;
    }

    return 0;
}

/**
 * @version 3.0
 * @brief Function to validate encoded recursive type.
 * @param encoded_type Encoded recursive type
 * @return uint8_t 0-non valid, 1-valid
 */
uint8_t IsRecType(const uint8_t *encoded_type)
{

    switch (*encoded_type)
    {
    case 0x4E: // list subtype
    case 0x4F: // list final subtype
    case 0x50: // recursive subtype
    case 0x5E: // recursive final subtype array
    case 0x5F: // recursive final subtype struct
    case 0x60: // recursive final subtype function
        // valid recursive type
        return 1;
        break;
    default:
        return 0;
        break;
    }
}

/**
 * @version 3.0
 * @brief Function to check if encoded type is a storage type.
 * @param encoded_type Encoded type
 * @return uint8_t 0-non valid, 1-valid
 */
uint8_t IsStorageType(const uint8_t *encoded_type)
{
    // check is value type
    if (IsValType(encoded_type))
        return 1;

    switch (*encoded_type)
    {
    case 0x77:
    case 0x78:
        return 1;
        break;
    default:
        return 0;
        break;
    }
}

/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/// Functions to get webasembly's list item by index ///////////////////////////////////////////////////////////////////////////////////
/**
 * @version 3.0
 * @brief Function to get a subtype fron a list.
 * subtype ::= 0x4F 𝑥*:list(typeidx) ct:comptype ⇒ sub final 𝑥* ct
 * | 0x50 𝑥*:list(typeidx) ct:comptype ⇒ sub 𝑥* ct
 * | ct:comptype ⇒ sub final 𝜖 ct
 * @param buf. Pointer to 0x4F or 0x50 byte or comptype.
 * @param subtype_index. index starting at 0;
 * @param const uint8_t * Pointer to where the subtype start.
 */
const uint8_t *GetSubTypeByIndex(const uint8_t *buf, uint32_t subtype_index)
{

    const uint8_t *index = buf;
    uint32_t i;

    for (i = 0; i < subtype_index; i++)
    {
        if (i == subtype_index)
        {
            return index;
        }

        index = SkipRecSubType(index);
    }

    return 0;
}

/**
 * @version 3.0
 * @brief function to get a type by index for module's version 1
 * @param typesec. Pointer to the WasmBinSection containing the type section.
 * start at type's list len.
 * @param type_index index of the function type to get
 * @return uint8_t* pointer to the function type (0x60) or NULL if not found
 *
 * @example
 *  WasmBinSection type_section = { .size = ..., .content = ... };
    uint32_t func_type_index = 2; // Example index
    const uint8_t *func_type_ptr = GetFuncTypeByIndex(type_section, func_type_index);
    if (func_type_ptr) {
        printf("Function type found at address: %p\n", func_type_ptr);
    } else {
        printf("Function type not found for index %u\n", func_type_index);
    }
 */
const uint8_t *GetTypeByIndex(WasmBinSection typesec, uint32_t type_index)
{

    assert(typesec.content != NULL); // Check if typesec content is not NULL

    const uint8_t *index = typesec.content;        // pointer to byte to traverse the binary file
    const uint8_t *buf_end = index + typesec.size; // pointer to end of section
    uint32_t length;                               // auxiliary variable
    uint32_t dec_u32;                              // auxiliary variable to decode leb128 values
    uint8_t encoded_type;

    // get list length
    index = DecodeLeb128UInt32(index, &length);
    if (!index)
    {
        return NULL;
    }

    // Check if type_index is valid
    if (type_index >= length)
    {
        return NULL; // Invalid type index
    }

#define READ_BYTE() (*index++)
#define NOT_END() (index < buf_end)

    for (uint32_t i = 0; i < length; i++)
    {

        // get recursive type
        encoded_type = READ_BYTE();
        // if (!IsRecType(encoded_type)){
        //     return NULL;
        // }

        if (i == type_index)
        {
            return index - 1; // Return pointer to type
        }

        if (encoded_type == 0x4E)
        {
            // 0x4E st*:list(subtype)
            // get type count
            index = DecodeLeb128UInt32(index, &dec_u32);
            if (!index)
            {
                return NULL;
            }
            for (uint32_t ii = 0; ii < dec_u32; ii++)
            {
                // get type
                encoded_type = READ_BYTE();

                index = SkipRecSubType(index);
                if (!index)
                {
                    return NULL;
                }
            }
        }
        else
        {
            index--;
            index = SkipRecSubType(index);
            if (!index)
            {
                return NULL;
            }
        }

        if (index > buf_end)
        {
            return NULL; // If index exceeds buffer end, return NULL
        }
    }

    return NULL; // If we reach here, type_index was not found

#undef READ_BYTE
#undef NOT_END
}

/**
 * @version 3.0
 * @brief function to get an import by index
 * @param importsec. Pointer to the WasmBinSection containing the import section.
 * @param import_index index of the import to get
 * @return uint8_t* pointer to the import or NULL if not found
 */
const uint8_t *GetImportByIndex(WasmBinSection importsec, uint32_t import_index)
{

    assert(importsec.content != NULL); // Check if importsec content is not NULL

    const uint8_t *index = importsec.content;        // pointer to byte to traverse the binary file
    const uint8_t *buf_end = index + importsec.size; // pointer to end of section
    uint32_t length;                                 // auxiliary variable
    uint32_t dec_u32;                                // auxiliary variable to decode leb128 values
    uint8_t encoded_type;

    // get vector length
    index = DecodeLeb128UInt32(index, &length);
    if (!index)
    {
        return NULL;
    }

    // Check if import_index is valid
    if (import_index >= length)
    {
        return NULL; // Invalid import index
    }

#define READ_BYTE() (*index++)
#define NOT_END() (index < buf_end)

    for (uint32_t i = 0; i < length; i++)
    {

        if (i == import_index)
        {
            return index; // Return pointer to the start of the import type 0x02
        }

        // get module name's vector
        index = DecodeLeb128UInt32(index, &dec_u32);
        if (!index)
        {
            return NULL;
        }
        index += dec_u32; // Skip over the module name vector

        // get name's vector
        index = DecodeLeb128UInt32(index, &dec_u32);
        if (!index)
        {
            return NULL;
        }
        index += dec_u32; // Skip over the name vector

        encoded_type = READ_BYTE(); // Read the type byte
        switch (encoded_type)
        {
        case 0:
            index = DecodeLeb128UInt32(index, &dec_u32); // Function index
            if (!index)
            {
                return NULL;
            }
            index += dec_u32; // Skip over the function index
            break;
        case 1:
            index++;                         // Skip reftype byte
            index = SkipLimitTypeBuf(index); // Table limits
            if (!index)
            {
                return NULL;
            }
            break;
        case 2:
            index = SkipLimitTypeBuf(index); // Memory limits
            if (!index)
            {
                return NULL;
            }
            break;
        case 3:
            encoded_type = READ_BYTE(); // Global type

            index++; // Skip mutability byte
            break;
        case 4: // New in WebAssembly 3.0
            // Handle the new import type accordingly
            // For example, if it's a tag type:
            encoded_type = READ_BYTE(); // Read tag type
            // if(encoded_type != 0x00){
            // return NULL; // Invalid tag type
            //}
            index = DecodeLeb128UInt32(index, &dec_u32); // Tag index
            if (!index)
            {
                return NULL;
            }
            index += dec_u32; // Skip over the tag index
            break;
        default:
            return NULL; // Invalid import type}
        }

        if (index > buf_end)
        {
            return NULL; // If index exceeds buffer end, return NULL
        }
    }
    return NULL; // never reach this line
}

/**
 * @version 3.0
 * @brief function to get a function by index
 * @param functionsec. Pointer to the WasmBinSection containing the function section.
 * @param function_index index of the function to get
 * @return uint8_t* pointer to the function type index or NULL if not found
 */
const uint8_t *GetFunctionByIndex(WasmBinSection functionsec, uint32_t function_index)
{

    assert(functionsec.content != NULL);

    const uint8_t *index = functionsec.content;        // pointer to byte to traverse the binary file
    const uint8_t *buf_end = index + functionsec.size; // pointer to end of binary module
    uint32_t length;                                   // auxiliary variable
    uint32_t dec_u32;                                  // auxiliary variable to decode leb128 values
    uint8_t byte_val;

    // get functions count
    index = DecodeLeb128UInt32(index, &length);
    if (!index)
    {
        return NULL;
    }
    // Check if import_index is valid
    if (function_index >= length)
    {
        return NULL; // Invalid function index
    }

#define READ_BYTE() (*index++)
#define NOT_END() (index < buf_end)

    for (uint32_t i = 0; i < length; i++)
    {

        if (function_index == i)
        {
            return index;
        }
        // get index
        index = DecodeLeb128UInt32(index, &dec_u32);
        if (!index)
        {
            return NULL;
        }
        if (index > buf_end)
        {
            return NULL; // If index exceeds buffer end, return NULL
        }
    }

    return NULL; // never reach this line

#undef READ_BYTE
#undef NOT_END
}

/**
 * @version v3.0
 * @brief function to get a table by index
 * @param tablesec. Pointer to the WasmBinSection containing the table section.
 * @param table_index index of the table to get
 * @return uint8_t* pointer to the table type (0x70 or 0x6F) or NULL if not found
 */
const uint8_t *GetTableByIndex(WasmBinSection tablesec, uint32_t table_index)
{

    assert(tablesec.content != NULL);               // Check if tablesec content is not NULL
    const uint8_t *index = tablesec.content;        // pointer to byte to traverse the binary file
    const uint8_t *buf_end = index + tablesec.size; // pointer to end of binary module
    uint32_t length;                                // auxiliary variable
    uint32_t dec_u32;                               // auxiliary variable to decode leb128 values
    uint8_t byte_val;

    // get table count
    index = DecodeLeb128UInt32(index, &length);
    if (!index)
    {
        return NULL;
    }
    // Check if table_index is valid
    if (table_index >= length)
    {
        return NULL; // Invalid table index
    }

#define READ_BYTE() (*index++)
#define NOT_END() (index < buf_end)

    for (uint32_t i = 0; i < length; i++)
    {

        // get reference type       0x6F->external, 0x70->function ref
        byte_val = READ_BYTE();
        switch (byte_val)
        {
        case 0x40:
            if (i == table_index)
            {
                return index - 1; // Return pointer to the start of the table type 0x70 or 0x6F
            }
            byte_val = READ_BYTE(); // Read the next byte (0x00)
            // Skip the table type
            index = SkipTableTypeBuf(index);
            if (index == NULL)
            {
                return NULL;
            }
            // skip expr buffer
            index = SkipExprBuf(index, buf_end - index);
            if (index == NULL)
            {
                return NULL;
            }
            break;
        case 0x63:
        case 0x64:
        case 0x69:
        case 0x6A:
        case 0x6B:
        case 0x6C:
        case 0x6D:
        case 0x6E:
        case 0x6F:
        case 0x70:
        case 0x71:
        case 0x72:
        case 0x73:
        case 0x74:
            index--; // Move back to the start of the reference type
            if (i == table_index)
            {
                return index; // Return pointer to the start of the table type 0x70 or 0x6F
            }
            // Skip the table type
            index = SkipTableTypeBuf(index);
            if (index == NULL)
            {
                return NULL;
            }
            break;
        default:
            // Invalid reference type
            return NULL; // Invalid reference type
            break;
        }
        if (i == table_index)
        {
            if (byte_val != 0x6F && byte_val != 0x70)
            {
                return NULL; // Invalid reference type
            }
            return index - 1; // Return pointer to the start of the table type 0x70 or 0x6F
        }

        index = SkipLimitTypeBuf(index);
        if (index == NULL)
        {
            return NULL;
        }

        if (index >= buf_end)
        {
            return NULL;
        }
    }

    return NULL; // If we reach here, table_index was not found

#undef READ_BYTE
#undef NOT_END
}

/**
 * @version v3.0
 * @brief function to get a memory by index
 * @param memsec. Pointer to the WasmBinSection containing the memory section.
 * @param mem_index index of the memory to get
 * @return uint8_t* pointer to the memory type or NULL if not found
 */
const uint8_t *GetMemByIndex(WasmBinSection memsec, uint32_t mem_index)
{

    assert(memsec.content != NULL);               // Check if memsec content is not NULL
    const uint8_t *index = memsec.content;        // pointer to byte to traverse the binary file
    const uint8_t *buf_end = index + memsec.size; // pointer to end of binary module
    uint32_t length;

    // get mem count
    index = DecodeLeb128UInt32(index, &length);
    if (!index)
    {
        return NULL;
    }
    // Check if mem_index is valid
    if (mem_index >= length)
    {
        return NULL; // Invalid memory index
    }

#define READ_BYTE() (*index++)
#define NOT_END() (index < buf_end)

    for (uint32_t i = 0; i < length; i++)
    {

        if (i == mem_index)
        {
            return index; // Return pointer to the start of the memory type
        }

        index = SkipLimitTypeBuf(index);
        if (index == NULL)
        {
            return NULL;
        }

        if (index >= buf_end)
        {
            return NULL;
        }
    }

    return NULL; // If we reach here, mem_index was not found

#undef READ_BYTE
#undef NOT_END
}

/**
 * @version 3.0
 * @brief function to get a global by index
 * @param globalsec. Pointer to the WasmBinSection containing the global section.
 * @param global_index index of the global to get
 * @return uint8_t* pointer to the global type or NULL if not found
 */
const uint8_t *GetGlobalByIndex(WasmBinSection globalsec, uint32_t global_index)
{

    assert(globalsec.content != NULL); // Check if globalsec content is not NULL

    const uint8_t *index = globalsec.content;        // pointer to byte to traverse the binary file
    const uint8_t *buf_end = index + globalsec.size; // pointer to end of section
    uint32_t length;                                 // auxiliary variable
    uint32_t dec_u32;                                // auxiliary variable to decode leb128 values
    uint8_t encoded_type;
    uint8_t encoded_mut;

    // get vector length
    index = DecodeLeb128UInt32(index, &length);
    if (!index)
    {
        return NULL;
    }

    // Check if global_index is valid
    if (global_index >= length)
    {
        return NULL; // Invalid global index
    }

#define READ_BYTE() (*index++)
#define NOT_END() (index < buf_end)

    for (uint32_t i = 0; i < length; i++)
    {

        if (i == global_index)
        {
            return index; // Return pointer to the start of the global type
        }

        index = SkipValTypeBuf(index); // get type
        if (!index)
        {
            return NULL;
        }
        // get mutability
        encoded_mut = READ_BYTE(); // get mutability

        if (encoded_mut == 0 || encoded_mut == 1)
        {
            index++; // mutability byte is not present
        }

        index = SkipExprBuf(index, buf_end - index); // Skip over the expression

        if (index > buf_end)
        {
            return NULL; // If index exceeds buffer end, return NULL
        }
    }

    return NULL; // If we reach here, global_index was not found

#undef READ_BYTE
#undef NOT_END
}

/**
 * @version 3.0
 * @brief function to get an export by index
 * @param exportsec. Pointer to the WasmBinSection containing the export section.
 * @param export_index index of the export to get
 * @return uint8_t* pointer to the export or NULL if not found
 */
const uint8_t *GetExportByIndex(WasmBinSection exportsec, uint32_t export_index)
{

    assert(exportsec.content != NULL); // Check if exportsec content is not NULL

    const uint8_t *index = exportsec.content;        // pointer to byte to traverse the binary file
    const uint8_t *buf_end = index + exportsec.size; // pointer to end of section
    uint32_t length;                                 // auxiliary variable
    uint32_t dec_u32;

    // get vector length
    index = DecodeLeb128UInt32(index, &length);
    if (!index)
    {
        return NULL;
    }

    // Check if export_index is valid
    if (export_index >= length)
    {
        return NULL; // Invalid export index
    }

#define READ_BYTE() (*index++)
#define NOT_END() (index < buf_end)

    for (uint32_t i = 0; i < length; i++)
    {

        if (i == export_index)
        {
            return index; // Return pointer to the start of the export type
        }

        // get name's vector
        index = DecodeLeb128UInt32(index, &dec_u32);
        if (!index)
        {
            return NULL;
        }
        index += dec_u32; // Skip over the name vector

        index++; // skip export description type byte
        // skip index
        index = DecodeLeb128UInt32(index, &dec_u32);
        if (!index)
        {
            return NULL;
        }

        if (index > buf_end)
        {
            return NULL; // If index exceeds buffer end, return NULL
        }
    }
    return NULL; // If we reach here, export_index was not found
#undef READ_BYTE
#undef NOT_END
}

/**
 * @version 3.0
 * @brief function to get an element by index
 * @param elemsec. Pointer to the WasmBinSection containing the element section.
 * @param elem_index index of the element to get
 * @return uint8_t* pointer to the element or NULL if not found
 */
const uint8_t *GetElementByIndex(WasmBinSection elemsec, uint32_t elem_index)
{

    assert(elemsec.content != NULL); // Check if elemsec content is not NULL

    const uint8_t *index = elemsec.content;        // pointer to byte to traverse the binary file
    const uint8_t *buf_end = index + elemsec.size; // pointer to end of section
    uint32_t length;                               // auxiliary variable
    uint32_t vec_len;                              // auxiliary variable to decode leb128 values
    uint32_t dec_u32;                              // auxiliary variable to decode leb128 values
    uint8_t byte_val;

    // get vector length
    index = DecodeLeb128UInt32(index, &length);
    if (!index)
    {
        return NULL;
    }

    // Check if elem_index is valid
    if (elem_index >= length)
    {
        return NULL; // Invalid element index
    }

#define READ_BYTE() (*index++)
#define NOT_END() (index < buf_end)

    for (uint32_t i = 0; i < length; i++)
    {

        if (i == elem_index)
        {
            return index; // Return pointer to the start of the element type
        }

        // get type
        index = DecodeLeb128UInt32(index, &dec_u32);
        if (!index)
        {
            return NULL;
        }

        switch (dec_u32)
        {
        case 0: // 0:u32 𝑒:expr 𝑦*:list(funcidx)
            index = SkipExprBuf(index, buf_end - index);
            if (!index)
            {
                return NULL;
            }
            // get vector y*
            index = DecodeLeb128UInt32(index, &vec_len);
            if (!index)
            {
                return NULL;
            }
            for (uint32_t ii = 0; i < vec_len; ii++)
            {
                index = DecodeLeb128UInt32(index, &dec_u32);
                if (!index)
                {
                    return NULL;
                }
            }
            break;
        case 1:      // 1:u32 et : elemkind 𝑦*:list(funcidx)
            index++; // read elemnt kind
            // get vector y*
            index = DecodeLeb128UInt32(index, &vec_len);
            if (!index)
            {
                return NULL;
            }
            for (uint32_t ii = 0; i < vec_len; ii++)
            {
                index = DecodeLeb128UInt32(index, &dec_u32);
                if (!index)
                {
                    return NULL;
                }
            }
            break;
        case 2:                                          // 2:u32 𝑥:tableidx 𝑒:expr et : elemkind 𝑦*:vec(funcidx)
            index = DecodeLeb128UInt32(index, &dec_u32); // Skip table index and expression
            if (!index)
            {
                return NULL;
            }
            // get expresion
            index = SkipExprBuf(index, buf_end - index);
            if (!index)
            {
                return NULL;
            }
            // get element kind
            index++; // Skip over the element kind byte
            // get vector y*
            index = DecodeLeb128UInt32(index, &vec_len);
            if (!index)
            {
                return NULL;
            }
            for (uint32_t ii = 0; i < vec_len; ii++)
            {
                index = DecodeLeb128UInt32(index, &dec_u32);
                if (!index)
                {
                    return NULL;
                }
            }
            break;
        case 3:      // 3:u32 et : elemkind 𝑦*:list(funcidx)
            index++; // Skip over the element kind byte
            // get vector y*
            index = DecodeLeb128UInt32(index, &vec_len);
            if (!index)
            {
                return NULL;
            }
            for (uint32_t ii = 0; i < vec_len; ii++)
            {
                index = DecodeLeb128UInt32(index, &dec_u32);
                if (!index)
                {
                    return NULL;
                }
            }
            break;
        case 4: // 4:u32 𝑒:expr el*:list(expr)
            index = SkipExprBuf(index, buf_end - index);
            if (!index)
            {
                return NULL;
            }
            // get vector el
            index = DecodeLeb128UInt32(index, &vec_len);
            if (!index)
            {
                return NULL;
            }
            for (uint32_t ii = 0; i < vec_len; ii++)
            {
                index = SkipExprBuf(index, buf_end - index);
                if (!index)
                {
                    return NULL;
                }
            }
            break;
        case 5:      // 5:u32 et : reftype el*:vec(expr)
            index++; // Skip over the reference type byte
            // get el vector
            index = DecodeLeb128UInt32(index, &vec_len);
            if (!index)
            {
                return NULL;
            }
            for (uint32_t ii = 0; i < vec_len; ii++)
            {
                index = SkipExprBuf(index, buf_end - index);
                if (!index)
                {
                    return NULL;
                }
            }
            break;
        case 6:                                          // 6:u32 𝑥:tableidx 𝑒:expr et : reftype el*:vec(expr)
            index = DecodeLeb128UInt32(index, &dec_u32); // Skip table index and expression
            // get expresion
            index = SkipExprBuf(index, buf_end - index);
            if (!index)
            {
                return NULL;
            }
            index++; // Skip over the reference type
            // get el
            index = DecodeLeb128UInt32(index, &vec_len);
            if (!index)
            {
                return NULL;
            }
            for (uint32_t ii = 0; i < vec_len; ii++)
            {
                index = SkipExprBuf(index, buf_end - index);
                if (!index)
                {
                    return NULL;
                }
            }
            break;
        case 7:      // 7:u32 et : reftype el*:vec(expr)
            index++; // Skip over the reference type byte
            // get el
            index = DecodeLeb128UInt32(index, &vec_len);
            if (!index)
            {
                return NULL;
            }
            for (uint32_t ii = 0; i < vec_len; ii++)
            {
                index = SkipExprBuf(index, buf_end - index);
                if (!index)
                {
                    return NULL;
                }
            }
            break;
        default:
            return NULL;
        }

        if (index >= buf_end)
        {
            return NULL;
        }
    }
    return NULL;
#undef READ_BYTE
#undef NOT_END
}

/**
 * @version 3.0
 * @brief function to get a code by index
 * @param codesec. Pointer to the WasmBinSection containing the code section.
 * @param code_index index of the code to get
 * @return uint8_t* pointer to the code or NULL if not found
 */
const uint8_t *GetCodeByIndex(WasmBinSection codesec, uint32_t code_index)
{

    assert(codesec.content != NULL); // Check if codesec content is not NULL

    const uint8_t *index = codesec.content;        // pointer to byte to traverse the binary file
    const uint8_t *buf_end = index + codesec.size; // pointer to end of section
    uint32_t length;                               // auxiliary variable
    uint32_t dec_u32;                              // auxiliary variable to decode leb128 values

    // get vector length
    index = DecodeLeb128UInt32(index, &length);
    if (!index)
    {
        return NULL;
    }

    // Check if code_index is valid
    if (code_index >= length)
    {
        return NULL; // Invalid element index
    }

#define READ_BYTE() (*index++)
#define NOT_END() (index < buf_end)

    for (uint32_t i = 0; i < length; i++)
    {

        if (i == code_index)
        {
            return (uint8_t *)index; // Return pointer to the start of the element type
        }

        // get size of code
        index = DecodeLeb128UInt32(index, &dec_u32);
        if (!index)
        {
            return NULL;
        }

        index = index + dec_u32; // Move index forward by the size of the code

        if (index >= buf_end)
        {
            return NULL;
        }
    }

    return NULL;
#undef READ_BYTE
#undef NOT_END
}

/**
 * @version 3.0
 * @brief function to get a data by index
 * @param datasec. Pointer to the WasmBinSection containing the data section.
 * @param data_index index of the data to get
 * @return uint8_t* pointer to the data or NULL if not found
 */
const uint8_t *GetDataByIndex(WasmBinSection datasec, uint32_t data_index)
{

    assert(datasec.content != NULL); // Check if datasec content is not NULL

    const uint8_t *index = datasec.content;        // pointer to byte to traverse the binary file
    const uint8_t *buf_end = index + datasec.size; // pointer to end of section
    uint32_t length;                               // auxiliary variable
    uint32_t dec_u32;                              // auxiliary variable to decode leb128 values

    // get vector length
    index = DecodeLeb128UInt32(index, &length);
    if (!index)
    {
        return NULL;
    }

    // Check if data_index is valid
    if (data_index >= length)
    {
        return NULL; // Invalid element index
    }

#define READ_BYTE() (*index++)
#define NOT_END() (index < buf_end)

    for (uint32_t i = 0; i < length; i++)
    {

        if (i == data_index)
        {
            return index; // Return pointer to the start of the element type
        }

        // get mode
        index = DecodeLeb128UInt32(index, &dec_u32);
        if (!index)
        {
            return NULL;
        }

        switch (dec_u32)
        {
        case 0: // 0:u32 𝑒:expr 𝑏*:list(byte)
            index = SkipExprBuf(index, buf_end - index);
            if (!index)
            {
                return NULL;
            }
            // get vector b*
            index = DecodeLeb128UInt32(index, &dec_u32);
            if (!index)
            {
                return NULL;
            }
            index = index + dec_u32;
            break;
        case 1: // 1:u32 𝑏*:list(byte)
            // get vector b*
            index = DecodeLeb128UInt32(index, &dec_u32);
            if (!index)
            {
                return NULL;
            }
            index = index + dec_u32;
            break;
        case 2:                                          // 2:u32 𝑥:memidx 𝑒:expr 𝑏*:list(byte)
            index = DecodeLeb128UInt32(index, &dec_u32); // Skip
            if (!index)
            {
                return NULL;
            }
            // get expresion
            index = SkipExprBuf(index, buf_end - index);
            if (!index)
            {
                return NULL;
            }
            // get vector b*
            index = DecodeLeb128UInt32(index, &dec_u32);
            if (!index)
            {
                return NULL;
            }
            index = index + dec_u32;
            break;
        }

        if (index >= buf_end)
        {
            return NULL;
        }
    }
    return NULL;
#undef READ_BYTE
#undef NOT_END
}

/**
 * @version 3.0
 * @brief function to get a tag from tag list.
 * @param tagsec. Tag binary section for a module.
 * @param tag_index. Target index;
 * @return const uint8_t * Pointer to target tag or NULL if error.
 */
const uint8_t *GetTagByIndex(WasmBinSection tagsec, uint32_t tag_index)
{
    assert(tagsec.content != NULL); // Check if tagsec content is not NULL

    const uint8_t *index = tagsec.content;        // pointer to byte to traverse the binary file
    const uint8_t *buf_end = index + tagsec.size; // pointer to end of section
    uint32_t length;                              // auxiliary variable
    uint32_t dec_u32;                             // auxiliary variable to decode leb128 values

    // get vector length
    index = DecodeLeb128UInt32(index, &length);
    if (!index)
    {
        return NULL;
    }

    // Check if tag index is valid
    if (tag_index >= length)
    {
        return NULL; // Invalid element index
    }

#define READ_BYTE() (*index++)
#define NOT_END() (index < buf_end)

    for (uint32_t i = 0; i < length; i++)
    {

        if (i == tag_index)
        {
            return index; // Return pointer to the start of the element type
        }

        // skip 0x00 byte
        index++;
        // skip type index
        index = DecodeLeb128UInt32(index, &dec_u32);
        if (!index)
        {
            return NULL;
        }

        if (index >= buf_end)
        {
            return NULL;
        }
    }

    return NULL;
#undef READ_BYTE
#undef NOT_END
}

/// Destructuring Functions////////////////////////////////////////////////////////////////////////////////////////

/**
 * @version 3.0
 * @brief Decode a function type in param list and return list.
 * 0x60 𝑡* 1:resulttype 𝑡* 2:resulttype
 * @param index. Pointer to 0x60 Byte
 * @return WasbinFuncType
 */
WasmBinFuncType DestructureFunctionType(const uint8_t *index)
{
    WasmBinFuncType func_type;
    uint32_t dec_u32;

    // get type
    assert(*index == 0x60);
    index++;                                     // skip 0x60
    index = DecodeLeb128UInt32(index, &dec_u32); // read list length
    func_type.param_len = dec_u32;
    func_type.param_types = index;
    index += dec_u32;

    index = DecodeLeb128UInt32(index, &dec_u32);
    func_type.ret_len = dec_u32;
    func_type.ret_types = index;

    return func_type;
}

/**
 * @version 3.0
 * @brief Function to destructure Function section.
 * funcsec ::= 𝑥*:section3(list(typeidx))
 * @param index Pointer to a u32 data
 * @return uint32_t with encoded index.
 */
uint32_t DestructureFunctionIndex(const uint8_t *index)
{
    uint32_t dec_u32;
    index = DecodeLeb128UInt32(index, &dec_u32);
    assert(index != NULL);

    return dec_u32;
}

/**
 * @brief function to destructure a global entry from the global section of a binary webassembly module.
 * @param global_addr pointer to the global entry in the binary file
 * @return WasmBinGlobal structure containing the destructured global entry
 */
WasmBinGlobal DestructureGlobal(const uint8_t *global_addr)
{
    WasmBinGlobal global;
    const uint8_t *index = global_addr; // pointer to byte to traverse the binary file
    uint32_t dec_u32;                   // auxiliary variable to decode leb128 values

    // get type
    global.type = index;
    index++;

    // get mutability
    if (*index == 0x00 || *index == 0x01)
    {
        global.mut = *index;
        index++;
    }
    else
    {
        global.mut = 0; // default value for mutability
    }

    // get init expression
    global.init_expr = index;

    return global;
}

WasmBinMemory DestructureMemory(const uint8_t *memory_addr)
{
    WasmBinMemory memory;
    const uint8_t *index = memory_addr; // pointer to byte to traverse the binary file

    switch (*index)
    {
    case 0x00: // i32 only min
        memory.address_type = 32;
        index++;
        index = DecodeLeb128UInt64(index, &memory.page_size_min);
        memory.page_size_max = 65536;
        break;
    case 0x01: // i32 min and max
        memory.address_type = 32;
        index++;
        index = DecodeLeb128UInt64(index, &memory.page_size_min);
        index = DecodeLeb128UInt64(index, &memory.page_size_max);
        break;
    case 0x04: // i64 only min
        memory.address_type = 64;
        index++;
        index = DecodeLeb128UInt64(index, &memory.page_size_min);
        memory.page_size_max = 65536;
        break;
    case 0x05: // i64 min and max
        memory.address_type = 64;
        index++;
        index = DecodeLeb128UInt64(index, &memory.page_size_min);
        index = DecodeLeb128UInt64(index, &memory.page_size_max);
        break;
    default:
        memory.address_type = 0;        
        memory.page_size_min = 0;
        memory.page_size_max = 0;
        break;
    }

    return memory; 
}

WasmBinExport DestructureExport(const uint8_t *export_addr)
{
    WasmBinExport export;
    const uint8_t *index = export_addr;

    index = DecodeLeb128UInt32(index, &export.name_len);
    export.name = index;

    // move index to nex byte after name
    index += export.name_len;
    export.index_type = *index; // type of export
    index++;
    index = DecodeLeb128UInt32(index, &export.external_index);

    return export;
}

/**
 * @version 3.0
 * @brief function to destructure a code entry from the code section of a binary webassembly module.
 * func ::= loc**:list(locals) 𝑒:expr
 * @param code_addr pointer to where func start.
 * @return WasBinFunction struct
 */
WasmBinFunction DestructureCode(const uint8_t *code_addr)
{
    WasmBinFunction func;
    const uint8_t *index = code_addr; // pointer to byte to traverse the binary file
    const uint8_t *start = code_addr; // pointer to the start of the code entry
    uint32_t code_size;                 // auxiliary variable to decode leb128 values

    // get size of code
    index = DecodeLeb128UInt32(index, &code_size);
    if (!index)
    {
        // invalid code entry
        func.locals = NULL;
        func.body = NULL;
        func.end = NULL;
        return func;
    }

    func.locals = index; // pointer to the start of the locals vector
    // get size of locals vector
    index = SkipLocalBuf(index);

    func.body = index; // pointer to the start of the function body
    func.end = start + code_size; // pointer to the end of the function body
    if(*func.end != 0x0B){
        // if the byte after the function body is not 0x0B, then the code entry is invalid
        func.locals = NULL;
        func.body = NULL;
        func.end = NULL;
        return func;
    }

    return func;
}

WasmBinMemArg DestructureMemArg(const uint8_t *index)
{
    WasmBinMemArg arg;
    uint32_t aux_u32;

    index = DecodeLeb128UInt32(index, &aux_u32);
    arg.n = aux_u32;
    if((arg.n & 0x08) == 0x08){ // if the bit 6 is set, then there is an mem index field
        index = DecodeLeb128UInt32(index, &aux_u32);
        arg.x = aux_u32;
        index = DecodeLeb128UInt32(index, &aux_u32);
        arg.m = aux_u32;
    } else {
        arg.x = 0;
        index = DecodeLeb128UInt32(index, &aux_u32);
        arg.m = aux_u32;
    }
    return arg;
}

StackValType DestructureStackValType(const uint8_t *index)
{
    StackValType val_type;
    
    switch (*index)
    {   //TODO reference type 
        case 0x7F: // i32            
        case 0x7E: // i64            
        case 0x7D: // f32            
        case 0x7C: // f64
        case 0x7B: // v128
            val_type = *index;
            break;
        default:
            val_type= WAS_EX_VAL_TYPE_NULL; // Invalid value type
            break;
    }

    return val_type;
}