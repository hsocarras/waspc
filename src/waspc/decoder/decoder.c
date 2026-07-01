/**
 * @file decoder/decoder.c
 * @author your name (you@domain.com)
 * @version 0.2
 * @date 2024-08-21
 *
 * @brief This file contains the implementation of the functions to decode and destructure a binary encoded non terminal symbol.
 *
 * @copyright Copyright (c) 2026
 *
 */

#include "decoder/wasm_decoder.h"
#include "utils/leb128.h"

#include <assert.h>


/// Destructuring and DecodingFunctions////////////////////////////////////////////////////////////////////////////////////////

/**
 * @version 3.0
 * @brief Decode a function type in param list and return list.
 * 0x60 𝑡* 1:resulttype 𝑡* 2:resulttype
 * @param index. Pointer to 0x60 Byte
 * @return WasbinFuncType
 */
WasmBinFuncType DestructureFunctionType(const uint8_t *index)
{

    #if WASPC_CONFIG_DEV_FLAG == 1
    assert(index != NULL); // Check if index is not NULL
    #endif

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
    #if WASPC_CONFIG_DEV_FLAG == 1
    assert(index != NULL); // Check if index is not NULL
    #endif

    uint32_t dec_u32;
    index = DecodeLeb128UInt32(index, &dec_u32);
    assert(index != NULL);

    return dec_u32;
}

WasmBinImport DestructureImport(const uint8_t *import_addr)
{
    #if WASPC_CONFIG_DEV_FLAG == 1
    assert(import_addr != NULL); // Check if index is not NULL
    #endif

    WasmBinImport import;
    const uint8_t *index = import_addr; // pointer to byte to traverse the binary file  
    uint32_t dec_u32;                   // auxiliary variable to decode leb128 values   

    // get module name
    index = DecodeLeb128UInt32(index, &dec_u32);
    import.module_name_len = dec_u32;
    import.module_name = index;
    index += dec_u32; // move index to next byte after module name

    // get name
    index = DecodeLeb128UInt32(index, &dec_u32);
    import.name_len = dec_u32;
    import.name = index;
    index += dec_u32; // move index to next byte after name

    // get external type
    import.external_type = *index;
    index++;

    // get external index    
    import.external = index;

    return import;
}

/**
 * @brief function to destructure a global entry from the global section in binary webassembly module.
 * @param global_addr pointer to the global entry in the binary file
 * @return WasmBinGlobal structure containing the destructured global entry
 */
WasmBinGlobal DestructureGlobal(const uint8_t *global_addr)
{   
    #if WASPC_CONFIG_DEV_FLAG == 1
    assert(global_addr != NULL); // Check if index is not NULL
    #endif

    WasmBinGlobal global;
    const uint8_t *index = global_addr; // pointer to byte to traverse the binary file
    uint32_t dec_u32;                   // auxiliary variable to decode leb128 values

    // get type
    global.type = index;
    index++;    //TODO suport for reference types and multiple value types in global type

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

/**
 * @brief function to destructure a memory entry from the memory section in binary webassembly module.
 * @param memory_addr pointer to the memory entry in the binary file
 * @return WasmBinMemory structure containing the destructured memory entry
 */
WasmBinMemory DestructureMemory(const uint8_t *memory_addr)
{   
    #if WASPC_CONFIG_DEV_FLAG == 1
    assert(memory_addr != NULL); // Check if index is not NULL
    #endif

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

/**
 * @brief function to destructure an export entry from the export section in binary webassembly module.
 * @param export_addr pointer to the export entry in the binary file
 * @return WasmBinExport structure containing the destructured export entry
 */
WasmBinExport DestructureExport(const uint8_t *export_addr)
{
    #if WASPC_CONFIG_DEV_FLAG == 1
    assert(export_addr != NULL); // Check if index is not NULL
    #endif

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
    #if WASPC_CONFIG_DEV_FLAG == 1
    assert(code_addr != NULL); // Check if index is not NULL
    #endif

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

/**
 * @version 3.0
 * @brief function to destructure a memory argument from a memory instruction in binary webassembly module.
 * memarg ::= 𝑛:align 𝑚:offset 𝑥:memoryidx
 * @param index pointer to the start of the memory argument
 * @return WasmBinMemArg structure containing the destructured memory argument
 */
WasmBinMemArg DestructureMemArg(const uint8_t *index)
{
    #if WASPC_CONFIG_DEV_FLAG == 1
    assert(index != NULL); // Check if index is not NULL
    #endif

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

/**
 * @brief function to destructure a stack value type from the binary webassembly module.
 * @param index pointer to the start of the stack value type
 * @return StackValType structure containing the destructured stack value type
 */
StackValType DestructureStackValType(const uint8_t *index)
{   
    #if WASPC_CONFIG_DEV_FLAG == 1
    assert(index != NULL); // Check if index is not NULL
    #endif
    
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