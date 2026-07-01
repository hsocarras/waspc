/**
 * @file type_check.c
 * @brief Function to check the validity of encoded types in a binary webassembly module.
 */

 #include "decoder/wasm_decoder.h"
 #include "utils/leb128.h"

 #include <assert.h>
 #include <string.h>


// Utilitary functions /////////////////////////////////////////////////////////////////////////////////////////////////////////
/**
 * @version 3.0
 * @brief Function to  check if valtype is an abstract head type.
 * @param val_type Pointer to byte with encoded type
 * @return uint8_t 0-non valid, 1-valid.
 */
uint8_t IsAbsHeadType(const uint8_t *valtype)
{
    #if WASPC_CONFIG_DEV_FLAG == 1    
    assert(valtype != NULL); // Check if typesec content is not NULL
    #endif

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

/**
 * @version 3.0
 * @brief Function to  check if valtype is numeric type.
 * @param val_type Pointer to byte with encoded type
 * @return uint8_t 0-non valid, 1-valid.
 */
uint8_t IsNumericType(const uint8_t *valtype)
{   
    #if WASPC_CONFIG_DEV_FLAG == 1    
    assert(valtype != NULL); // Check if typesec content is not NULL
    #endif

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

/**
 * @version 3.0
 * @brief Function to check reference type
 * @param val_type Pointer to byte with encoded type
 * @return uint8_t 0-non valid, 1-valid
 */
uint8_t IsRefType(const uint8_t *valtype)
{   
    #if WASPC_CONFIG_DEV_FLAG == 1    
    assert(valtype != NULL); // Check if valtype content is not NULL
    #endif

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
    #if WASPC_CONFIG_DEV_FLAG == 1    
    assert(valtype != NULL); // Check if valtype content is not NULL
    #endif

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
    #if WASPC_CONFIG_DEV_FLAG == 1    
    assert(encoded_type != NULL); // Check if valtype content is not NULL
    #endif

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
    #if WASPC_CONFIG_DEV_FLAG == 1    
    assert(encoded_type != NULL); // Check if valtype content is not NULL
    #endif
    
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

/**
 * @version 3.0
 * @brief Compare two encoded WebAssembly function types.
 * @param func_type_a Pointer to the first encoded function type (0x60).
 * @param func_type_b Pointer to the second encoded function type (0x60).
 * @return uint8_t 1 if the function signatures match, 0 otherwise.
 */
uint8_t IsFuncTypeMatch(const uint8_t *func_type_a, const uint8_t *func_type_b)
{
    #if WASPC_CONFIG_DEV_FLAG == 1
    assert(func_type_a != NULL);
    assert(func_type_b != NULL);
    #endif

    if (!func_type_a || !func_type_b)
        return 0;

    if (*func_type_a != 0x60 || *func_type_b != 0x60)
        return 0;

    WasmBinFuncType a = DestructureFunctionType(func_type_a);
    WasmBinFuncType b = DestructureFunctionType(func_type_b);

    if (a.param_len != b.param_len)
        return 0;

    if (a.param_len > 0 && memcmp(a.param_types, b.param_types, a.param_len) != 0)
        return 0;

    if (a.ret_len != b.ret_len)
        return 0;

    if (a.ret_len > 0 && memcmp(a.ret_types, b.ret_types, a.ret_len) != 0)
        return 0;

    return 1;
}

uint8_t IsMemoryTypeMatch(const uint8_t * mem_type1, const uint8_t *mem_type2)
{
    #if WASPC_CONFIG_DEV_FLAG == 1
    assert(mem_type1 != NULL);
    assert(mem_type2 != NULL);
    #endif

    if (!mem_type1 || !mem_type2)
        return 0;
    // Memory types are encoded as limits, which consist of a flag byte followed by one or two LEB128-encoded integers.
    if(*mem_type1 != *mem_type2)
        return 0;

    uint8_t flag1 = *mem_type1;
    mem_type1++;
    mem_type2++;
    int64_t min1, min2, max1, max2;

    switch (flag1)
    {
    case 0x00:
        // Only minimum is specified
        mem_type1 = DecodeLeb128Int64(mem_type1, &min1);
        mem_type2 = DecodeLeb128Int64(mem_type2, &min2);
        if (min1 != min2)
            return 0;
        break;
    case 0x01:
        // Minimum and maximum are specified
        mem_type1 = DecodeLeb128Int64(mem_type1, &min1);
        mem_type1 = DecodeLeb128Int64(mem_type1, &max1);
        mem_type2 = DecodeLeb128Int64(mem_type2, &min2);
        mem_type2 = DecodeLeb128Int64(mem_type2, &max2);
        if (min1 != min2 || max1 != max2)
            return 0;
        break;
    case 0x04:
        // Only minimum is specified
        mem_type1 = DecodeLeb128Int64(mem_type1, &min1);
        mem_type2 = DecodeLeb128Int64(mem_type2, &min2);
        if (min1 != min2)
            return 0;
        break;
    case 0x05:
        // Nminimun and maximum are specified
        mem_type1 = DecodeLeb128Int64(mem_type1, &min1);
        mem_type1 = DecodeLeb128Int64(mem_type1, &max1);
        mem_type2 = DecodeLeb128Int64(mem_type2, &min2);
        mem_type2 = DecodeLeb128Int64(mem_type2, &max2);
        if (min1 != min2 || max1 != max2)
            return 0;
        break;
    default:
        return 0;
        break;    
    }
    

    return 1;
}