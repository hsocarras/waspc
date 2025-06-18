#include <gtest/gtest.h>
#include "config_test.h"
#include "utils/names.h"
#include "utils/hash_table.h"
#include "utils/leb128.h"
#include "../wasm/test1.h"
#include "validation/wasm_decoder.h"
#include "webassembly/structure/types.h"

#include <stdlib.h>

static const uint8_t * ReadBinSection(const uint8_t *index, WasmBinSection *sec){

    uint32_t aux_u32;
    index = DecodeLeb128UInt32(index, &aux_u32);
    if (!index){
        return NULL;                                       
    }

    sec->size = aux_u32;
    sec->content = index;
    index = index + aux_u32;

    return index;

}
 


TEST(WASPC_VALIDATION_DECODER, DECODE_TYPE_SECTION_1) {    
    
    WpModuleState mod;
    WpModuleInit(&mod); // Initialize the module state

    mod.buf = (const uint8_t *)test1; // Set the buffer to the test WASM file
    mod.bufsize = sizeof(test1); // Set the buffer size to the size of the test WASM file
    // allocate memory for decoded module
    mod.was = (WasModule *)malloc(sizeof(WasModule));
    ASSERT_NE(mod.was, nullptr) << "Failed to allocate memory for WasModule";
    mod.was->types.elements = nullptr; // Initialize the types vector to nullptr
    mod.was->types.lenght = 0; // Initialize the length of the types vector to 0

    const uint8_t *buf_end = mod.buf + mod.bufsize; // Pointer to the end of the buffer
    const uint8_t *index = mod.buf; 
    uint8_t section_id;

    index = index + 8; // Skip the magic number and version (8 bytes)
        
    
    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    section_id = READ_BYTE(); // Read the section ID
    ASSERT_EQ(section_id, WP_WSA_BIN_MOD_SEC_ID_TYPE) << "Expected section ID to be TYPE";
    // Decode the type section  
    index = ReadBinSection(index, &mod.typesec);
    ASSERT_NE(index, nullptr) << "Failed to read type section";
    VecFuncType *types = DecodeTypeSection(&mod);
    ASSERT_NE(types, nullptr) << "Failed to decode type section";
    ASSERT_EQ(types->lenght, 1) << "Expected 1 type in the type section";   
    ASSERT_NE(types->elements, nullptr) << "Types elements should not be null";
    ASSERT_EQ(types->elements[0].params.lenght, 1) << "Expected 1 parameters in the first type";
    ASSERT_EQ(types->elements[0].results.lenght, 1) << "Expected 1 result in the first type";
    ASSERT_EQ(types->elements[0].params.val_types[0], WAS_I32) << "Expected first parameter to be I32";
    ASSERT_EQ(types->elements[0].results.val_types[0], WAS_I32) << "Expected result to be I32";
    // Clean up
    free(mod.was); // Free the allocated memory for the decoded module

    #undef READ_BYTE
    #undef NOT_END
   
}