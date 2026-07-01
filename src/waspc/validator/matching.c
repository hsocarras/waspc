
#include "validator/wasm_validator.h"

//TODO implement type matchin funcions for other types (e.g. function types, global types, etc.)

/**
 * @version 3.0
 * @brief Compare two encoded WebAssembly result types. 3.3.6
 * @param result_type_a Pointer to the first encoded result type.
 * @param result_type_b Pointer to the second encoded result type.
 * @return uint8_t 1 if the result types match, 0 otherwise.
 */
uint8_t IsResultTypeMatch(const uint8_t *result_type_a, const uint8_t *result_type_b)
{
    #if WASPC_CONFIG_DEV_FLAG == 1
    assert(result_type_a != NULL);
    assert(result_type_b != NULL);
    #endif

    if (!result_type_a || !result_type_b)
        return 0;    

    const uint8_t * index_a = result_type_a;
    const uint8_t * index_b = result_type_b;
    uint32_t len_a = 0;
    uint32_t len_b = 0;
    
    index_a = DecodeLeb128UInt32(index_a, &len_a);    
    index_b = DecodeLeb128UInt32(index_b, &len_b);
    if(!index_a || !index_b)
        return 0;

    
    if (len_a != len_b)
        return 0;

    // Compare the actual result type bytes
    for (uint32_t i = 0; i < len_a; i++)
    {
        if (index_a[i] != index_b[i])
            return 0;
    }

    return 1;
}