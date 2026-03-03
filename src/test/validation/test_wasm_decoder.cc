#include <gtest/gtest.h>
#include "config_test.h"
#include "utils/leb128.h"
#include "validation/wasm_decoder.h"
#include "webassembly/structure/types.h"
#include "objects/module.h"
#include "../wasm/samples.h"


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
 

/**
 * @brief Function to decode the type section of a WebAssembly module
 * @param mod Pointer to the module state
 * @return VecFuncType* Pointer to the decoded function types
 */
TEST(WASPC_VALIDATION_DECODER, GET_FUNCTYPE_1) {    
    
    
    const uint8_t *index  = &sample1[0]; // Set the buffer to the test WASM file
    uint32_t len = sizeof(sample1); // Set the buffer size to the size of the test WASM file
    const uint8_t *buf_end = index + len; // Pointer to the end of the buffer    
    const uint8_t *functiontype_index;
    uint32_t functiontype_count;
    uint8_t section_id;
    WasmBinSection typesec;

    index = index + 8; // Skip the magic number and version (8 bytes)
     
    
    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    // Read the section ID index 0x08
    section_id = READ_BYTE(); 
    ASSERT_EQ(section_id, WP_WSA_BIN_MOD_SEC_ID_TYPE) << "Expected section ID to be TYPE";

    // Decode the type section  
    index = ReadBinSection(index, &typesec);
    ASSERT_NE(index, nullptr) << "Failed to read type section";
    ASSERT_EQ(typesec.content - sample1, 10) << "Expected function type section to start at byte 11 0x0A";
    ASSERT_EQ(typesec.size, 6) << "Expected type section size to be 6 bytes";

    // Get the function type count
    functiontype_index = typesec.content;
    functiontype_index = DecodeLeb128UInt32(functiontype_index, &functiontype_count);
    ASSERT_NE(functiontype_index, nullptr) << "Failed to read function type count";
    ASSERT_EQ(functiontype_count, 1) << "Expected 1 function type in the type section";

    functiontype_index = GetTypeByIndex(typesec, 0);
    ASSERT_NE(functiontype_index, nullptr) << "Failed to read function type at index 0";
    ASSERT_EQ(*functiontype_index, 0x60) << "Expected function type to be 0x60 (function type)";    
    ASSERT_EQ(functiontype_index - sample1, 0x0B) << "Expected function type 0 to start at 0x0b index"; 

    #undef READ_BYTE
    #undef NOT_END
   
}

/*
TEST(WASPC_VALIDATION_DECODER, DECODE_INPORT_SECTION_1){

}*/

/**
 * @brief Function to decode the function section of a WebAssembly module
 * This test checks if the function section is decoded correctly
 * and if the function types are correctly assigned.
 */
TEST(WASPC_VALIDATION_DECODER, GET_FUNCTION_1){


    const uint8_t *index  = &sample1[0]; // Set the buffer to the test WASM file
    uint32_t len = sizeof(sample1); // Set the buffer size to the size of the test WASM file
    const uint8_t *buf_end = index + len; // Pointer to the end of the buffer    
    const uint8_t *functiontype_index;
    uint32_t functype_count;
    uint32_t typeidx;
    uint8_t section_id;
    WasmBinSection typesec;
    WasmBinSection functionsec;

    index = index + 8; // Skip the magic number and version (8 bytes)
     
    
    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    section_id = READ_BYTE(); // Read the section ID index at index 0x08
    ASSERT_EQ(section_id, WP_WSA_BIN_MOD_SEC_ID_TYPE) << "Expected section ID to be TYPE";    
    index = ReadBinSection(index, &typesec);
    ASSERT_NE(index, nullptr) << "Failed to read type section";    
    

    section_id = READ_BYTE(); // Read the next section ID 0x03 at index 0x10
    ASSERT_EQ(section_id, WP_WSA_BIN_MOD_SEC_ID_FUNCTION) << "Expected section ID to be FUNCTION";
    // Decode the function section      
    index = ReadBinSection(index, &functionsec);
    ASSERT_NE(index, nullptr) << "Failed to read function section";
    ASSERT_EQ(functionsec.content - sample1, 0x12) << "Expected function section start at byte 18";
    ASSERT_EQ(functionsec.size, 2) << "Expected function section size to be 2 bytes";

    functiontype_index = DecodeLeb128UInt32(functionsec.content, &functype_count);    
    ASSERT_NE(functiontype_index, nullptr) << "Failed to read function type count";
    ASSERT_EQ(functype_count, 1) << "Expected 1 function in the function section";

    functiontype_index = GetFunctionByIndex(functionsec, 0);
    ASSERT_NE(functiontype_index, nullptr) << "Failed to read function at index 0";
    functiontype_index = DecodeLeb128UInt32(functiontype_index, &typeidx);
    ASSERT_NE(functiontype_index, nullptr) << "Failed to read function type index";
    ASSERT_EQ(typeidx, 0) << "Expected function type index to be 0";
    
    #undef READ_BYTE
    #undef NOT_END
    
}

/*
TEST(WASPC_VALIDATION_DECODER, DECODE_TABLE_SECTION_1){

}*/


/*
TEST(WASPC_VALIDATION_DECODER, DECODE_MEMORY_SECTION_1){

}*/

/**
 * @brief Function to decode the global section of a WebAssembly module
 * This test checks if the global section is decoded correctly
 * and if the global variables are correctly assigned.
 * This test assumes that the global section is present in the WASM file.
 * The global section is expected to contain one global variable of type I32.
 */
TEST(WASPC_VALIDATION_DECODER, GET_GLOBAL_1){
    
    const uint8_t *index  = &sample1[0]; // Set the buffer to the test WASM file
    uint32_t len = sizeof(sample1); // Set the buffer size to the size of the test WASM file
    const uint8_t *buf_end = index + len; // Pointer to the end of the buffer    
    const uint8_t *functiontype_index;
    uint32_t functype_count;
    uint32_t typeidx;
    uint8_t section_id;
    WasmBinSection typesec;
    WasmBinSection functionsec;
    WasmBinSection globalsec;

    index = index + 8; // Skip the magic number and version (8 bytes)

    
    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    //functype section
    section_id = READ_BYTE(); // Read the section ID index 0x08
    ASSERT_EQ(section_id, WP_WSA_BIN_MOD_SEC_ID_TYPE) << "Expected section ID to be TYPE";    
    index = ReadBinSection(index, &typesec);
    ASSERT_NE(index, nullptr) << "Failed to read type section";
    
    //function section
    section_id = READ_BYTE(); // Read the next section ID 0x10
    ASSERT_EQ(section_id, WP_WSA_BIN_MOD_SEC_ID_FUNCTION) << "Expected section ID to be FUNCTION";
    index = ReadBinSection(index, &functionsec);
    ASSERT_NE(index, nullptr) << "Failed to read function section";

    //global section
    section_id = READ_BYTE(); // Read the next section ID index 0x14
    ASSERT_EQ(section_id, WP_WSA_BIN_MOD_SEC_ID_GLOBAL) << "Expected section ID to be GLOBAL";
    index = ReadBinSection(index, &globalsec);  
    ASSERT_NE(index, nullptr) << "Failed to read global section";
    ASSERT_EQ(globalsec.content - sample1, 0x16) << "Expected global section to start at byte 22";
    ASSERT_EQ(globalsec.size, 6) << "Expected global section size to be 7 bytes";

    const uint8_t *global_index = GetGlobalByIndex(globalsec, 0);
    ASSERT_NE(global_index, nullptr) << "Failed to read global at index 0";
    ASSERT_EQ(*global_index, 0x7F) << "Expected global type to be I32 (0x7F)";

    #undef READ_BYTE
    #undef NOT_END
}


TEST(WASPC_VALIDATION_DECODER, GET_EXPORT_SECTION_1){

    const uint8_t *index  = &sample1[0]; // Set the buffer to the test WASM file
    uint32_t len = sizeof(sample1); // Set the buffer size to the size of the test WASM file
    const uint8_t *buf_end = index + len; // Pointer to the end of the buffer    
    const uint8_t *functiontype_index;
    uint32_t functype_count;
    uint32_t typeidx;
    uint8_t section_id;
    WasmBinSection typesec;
    WasmBinSection functionsec;
    WasmBinSection globalsec;
    WasmBinSection exportsec;

    index = index + 8; // Skip the magic number and version (8 bytes)
        
    
    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    section_id = READ_BYTE(); // Read the section ID index 0x08
    ASSERT_EQ(section_id, WP_WSA_BIN_MOD_SEC_ID_TYPE) << "Expected section ID to be TYPE";    
    index = ReadBinSection(index, &typesec);
    ASSERT_NE(index, nullptr) << "Failed to read type section";
    
    section_id = READ_BYTE(); // Read the next section ID 0x10
    ASSERT_EQ(section_id, WP_WSA_BIN_MOD_SEC_ID_FUNCTION) << "Expected section ID to be FUNCTION";
    index = ReadBinSection(index, &functionsec);
    ASSERT_NE(index, nullptr) << "Failed to read function section";

    section_id = READ_BYTE(); // Read the next section ID index 0x14
    ASSERT_EQ(section_id, WP_WSA_BIN_MOD_SEC_ID_GLOBAL) << "Failed to read global section";
    index = ReadBinSection(index, &globalsec);
    ASSERT_NE(index, nullptr) << "Failed to read global section";

    //Get Export section ID to be EXPORT
    section_id = READ_BYTE(); // Read the next section ID index 0x1C
    ASSERT_EQ(section_id, WP_WSA_BIN_MOD_SEC_ID_EXPORT) << "Expected section ID to be EXPORT";
    index = ReadBinSection(index, &exportsec);
    ASSERT_NE(index, nullptr) << "Failed to read export section";   
    ASSERT_EQ(exportsec.content - sample1, 0x1E) << "Expected export section to start at byte 28";
    ASSERT_EQ(exportsec.size, 8) << "Expected export section size to be 8 bytes";

    const uint8_t *export_index = GetExportByIndex(exportsec, 0);
    ASSERT_NE(export_index, nullptr) << "Failed to read export at index 0";
    uint32_t name_len;
    export_index = DecodeLeb128UInt32(export_index, &name_len);
    ASSERT_NE(export_index, nullptr) << "Failed to read export name length";
    ASSERT_EQ(name_len, 4) << "Expected export name length to be 4";
    ASSERT_EQ(export_index - sample1, 0x20) << "Expected export name to start at byte 32";
    ASSERT_EQ(strncmp((const char *)export_index, "main", 4), 0) << "Expected export name to be 'main'";
    
    
    #undef READ_BYTE
    #undef NOT_END
}

TEST(WASPC_VALIDATION_DECODER, GET_CODE_1){

    const uint8_t *index  = &sample1[0]; // Set the buffer to the test WASM file
    uint32_t len = sizeof(sample1); // Set the buffer size to the size of the test WASM file
    const uint8_t *buf_end = index + len; // Pointer to the end of the buffer    
    const uint8_t *functiontype_index;
    uint32_t functype_count;
    uint32_t typeidx;
    uint8_t section_id;
    WasmBinSection typesec;
    WasmBinSection functionsec;
    WasmBinSection globalsec;
    WasmBinSection exportsec;
    WasmBinSection codesec;
    

    index = index + 8; // Skip the magic number and version (8 bytes)
        
    
    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    section_id = READ_BYTE(); // Read the section ID index 0x08
    ASSERT_EQ(section_id, WP_WSA_BIN_MOD_SEC_ID_TYPE) << "Expected section ID to be TYPE";    
    index = ReadBinSection(index, &typesec);
    ASSERT_NE(index, nullptr) << "Failed to read type section";
    
    section_id = READ_BYTE(); // Read the next section ID 0x10
    ASSERT_EQ(section_id, WP_WSA_BIN_MOD_SEC_ID_FUNCTION) << "Expected section ID to be FUNCTION";
    index = ReadBinSection(index, &functionsec);
    ASSERT_NE(index, nullptr) << "Failed to read function section";    

    section_id = READ_BYTE(); // Read the next section ID index 0x14
    ASSERT_EQ(section_id, WP_WSA_BIN_MOD_SEC_ID_GLOBAL) << "Failed to read global section";
    index = ReadBinSection(index, &globalsec);
    ASSERT_NE(index, nullptr) << "Failed to read global section";

    //Decode Export section ID to be EXPORT
    section_id = READ_BYTE(); // Read the next section ID index 0x1C
    ASSERT_EQ(section_id, WP_WSA_BIN_MOD_SEC_ID_EXPORT) << "Expected section ID to be EXPORT";
    index = ReadBinSection(index, &exportsec);
    ASSERT_NE(index, nullptr) << "Failed to read export section";
    
    // Decode the code section    
    section_id = READ_BYTE(); // Read the next section ID index 0x26
    ASSERT_EQ(section_id, WP_WSA_BIN_MOD_SEC_ID_CODE) << "Expected section ID to be CODE";
    index = ReadBinSection(index, &codesec);    
    ASSERT_NE(index, nullptr) << "Failed to read code section";
    ASSERT_EQ(codesec.content - sample1, 0x28) << "Expected code section to start at byte 40";
    ASSERT_EQ(codesec.size, 22) << "Expected code section size to be 22 bytes";

    const uint8_t *code_index = GetCodeByIndex(codesec, 0);
    ASSERT_NE(code_index, nullptr) << "Failed to read code at index 0";
    uint32_t code_size;
    code_index = DecodeLeb128UInt32(code_index, &code_size);
    ASSERT_NE(code_index, nullptr) << "Failed to read code size";
    ASSERT_EQ(code_size, 20) << "Expected code size to be 20 bytes";
    ASSERT_EQ(code_index - sample1, 0x2A) << "Expected code to start at byte 42";
    
    #undef READ_BYTE
    #undef NOT_END
}