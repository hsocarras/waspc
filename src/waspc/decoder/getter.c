/**
 * @file decoder/getter.c
 * @author your name (you@domain.com)
 * @version 0.2
 * @date 2024-08-21
 *
 * @brief This file contains the implementation of the functions to get a binary encoded non terminal symbol 
 * from the binary webassembly module.
 *
 * @copyright Copyright (c) 2026
 *
 */

#include "decoder/wasm_decoder.h"
#include "utils/leb128.h"
#include "webassembly/instructions.h"

#include <assert.h>


/**
 * @version 3.0
 * @brief function to get an import by index. import ::= nm1:name nm2:name xt:externtype
 * @param importsec. Pointer to the WasmBinSection containing the import section's content..
 * @param import_index index of the import to get
 * @return uint8_t* pointer to the import or NULL if not found
 */
const uint8_t *GetImportByIndex(WasmBinSection importsec, uint32_t import_index)
{
    #if WASPC_CONFIG_DEV_FLAG == 1
    assert(importsec.content != NULL); // Check if importsec content is not NULL
    #endif

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

    #if WASPC_CONFIG_DEV_FLAG == 1
    assert(functionsec.content != NULL); // Check if importsec content is not NULL
    #endif

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
    #if WASPC_CONFIG_DEV_FLAG == 1
    assert(tablesec.content != NULL); // Check if importsec content is not NULL
    #endif
    
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
            index = SkipExprBuf(index, buf_end);
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

    #if WASPC_CONFIG_DEV_FLAG == 1
    assert(memsec.content != NULL); // Check if importsec content is not NULL
    #endif

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

    #if WASPC_CONFIG_DEV_FLAG == 1
    assert(globalsec.content != NULL); // Check if importsec content is not NULL
    #endif

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

        index = SkipExprBuf(index, buf_end); // Skip over the expression

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
    #if WASPC_CONFIG_DEV_FLAG == 1
    assert(exportsec.content != NULL); // Check if exportsec content is not NULL
    #endif

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
    #if WASPC_CONFIG_DEV_FLAG == 1
    assert(elemsec.content != NULL); // Check if elemsec content is not NULL
    #endif

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
            index = SkipExprBuf(index, buf_end);
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
            index = SkipExprBuf(index, buf_end);
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
            index = SkipExprBuf(index, buf_end);
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
                index = SkipExprBuf(index, buf_end);
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
                index = SkipExprBuf(index, buf_end);
                if (!index)
                {
                    return NULL;
                }
            }
            break;
        case 6:                                          // 6:u32 𝑥:tableidx 𝑒:expr et : reftype el*:vec(expr)
            index = DecodeLeb128UInt32(index, &dec_u32); // Skip table index and expression
            // get expresion
            index = SkipExprBuf(index, buf_end);
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
                index = SkipExprBuf(index, buf_end);
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
                index = SkipExprBuf(index, buf_end);
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
    #if WASPC_CONFIG_DEV_FLAG == 1
    assert(codesec.content != NULL); // Check if codesec content is not NULL
    #endif

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
    #if WASPC_CONFIG_DEV_FLAG == 1
    assert(datasec.content != NULL); // Check if datasec content is not NULL
    #endif

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
            index = SkipExprBuf(index, buf_end);
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
            index = SkipExprBuf(index, buf_end);
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
    #if WASPC_CONFIG_DEV_FLAG == 1
    assert(tagsec.content != NULL); // Check if tagsec content is not NULL
    #endif

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


 /* @version 3.0
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

    #if WASPC_CONFIG_DEV_FLAG == 1
    assert(buf != NULL); // Check if tagsec content is not NULL
    #endif

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
    #if WASPC_CONFIG_DEV_FLAG == 1    
    assert(typesec.content != NULL); // Check if typesec content is not NULL
    #endif
    
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

