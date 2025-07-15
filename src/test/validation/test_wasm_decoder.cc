#include <gtest/gtest.h>
#include "config_test.h"
#include "utils/names.h"
#include "utils/hash_table.h"
#include "utils/leb128.h"
#include "../wasm/test1.h"
#include "validation/wasm_decoder.h"
#include "webassembly/structure/types.h"
#include "objects/module.h"

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
TEST(WASPC_VALIDATION_DECODER, DECODE_TYPE_SECTION_1) {    
    
    WpModuleState mod;
    WpModuleInit(&mod); // Initialize the module state
    ASSERT_EQ(mod.status, WP_MODULE_STATUS_INIT) << "Module status should be INIT";
    ASSERT_NE(mod.was, nullptr) << "Failed to allocate memory for WasModule";
    ASSERT_EQ(mod.was->types.elements, nullptr) << "Expected 0 type in init state"; // Check if types are initialized to nullptr
    ASSERT_EQ(mod.was->types.lenght, 0) << "Expected 0 type in init state"; // Check if types length is initialized to 0
    
    mod.buf = (const uint8_t *)test1; // Set the buffer to the test WASM file
    mod.bufsize = sizeof(test1); // Set the buffer size to the size of the test WASM file
    const uint8_t *buf_end = mod.buf + mod.bufsize; // Pointer to the end of the buffer
    const uint8_t *index = mod.buf; 
    uint8_t section_id;

    index = index + 8; // Skip the magic number and version (8 bytes)
        
    
    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    section_id = READ_BYTE(); // Read the section ID index 0x08
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

/*
TEST(WASPC_VALIDATION_DECODER, DECODE_INPORT_SECTION_1){

}*/

/**
 * @brief Function to decode the function section of a WebAssembly module
 * This test checks if the function section is decoded correctly
 * and if the function types are correctly assigned.
 */
TEST(WASPC_VALIDATION_DECODER, DECODE_FUNCTION_SECTION_1){
    
    WpModuleState mod;
    WpModuleInit(&mod); // Initialize the module state
    ASSERT_EQ(mod.status, WP_MODULE_STATUS_INIT) << "Module status should be INIT";
    ASSERT_NE(mod.was, nullptr) << "Failed to allocate memory for WasModule";
    ASSERT_EQ(mod.was->funcs.lenght, 0) << "Expected 0 function in init state";
    ASSERT_EQ(mod.was->funcs.elements, nullptr) << "Expected 0 function in init state";

    mod.buf = (const uint8_t *)test1; // Set the buffer to the test WASM file
    mod.bufsize = sizeof(test1); // Set the buffer size to the size of the test WASM file
    mod.was->funcs.elements = nullptr; // Initialize the funcs vector to nullptr
    mod.was->funcs.lenght = 0; // Initialize the length of the funcs vector to 0

    const uint8_t *buf_end = mod.buf + mod.bufsize; // Pointer to the end of the buffer
    const uint8_t *index = mod.buf; 
    uint8_t section_id;

    index = index + 8; // Skip the magic number and version (8 bytes)
        
    
    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    section_id = READ_BYTE(); // Read the section ID index 0x08
    ASSERT_EQ(section_id, WP_WSA_BIN_MOD_SEC_ID_TYPE) << "Expected section ID to be TYPE";    
    index = ReadBinSection(index, &mod.typesec);
    ASSERT_NE(index, nullptr) << "Failed to read type section";    
    VecFuncType *types = DecodeTypeSection(&mod);

    section_id = READ_BYTE(); // Read the next section ID 0x10
    ASSERT_EQ(section_id, WP_WSA_BIN_MOD_SEC_ID_FUNCTION) << "Expected section ID to be FUNCTION";
    // Decode the function section      
    index = ReadBinSection(index, &mod.functionsec);
    ASSERT_NE(index, nullptr) << "Failed to read function section";
    
    VecFunc *funcs = DecodeFunctionSection(&mod);
    ASSERT_NE(funcs, nullptr) << "Failed to decode function section";   
    ASSERT_EQ(funcs->lenght, 1) << "Expected 1 function in the function section";
    ASSERT_NE(funcs->elements, nullptr) << "Function elements should not be null";  
    ASSERT_EQ(funcs->elements[0].type_index, 0) << "Expected first function type index to be 0";
    ASSERT_EQ(funcs->elements[0].locals.lenght, 0) << "Expected first function do not have  local variables yrt";
    ASSERT_EQ(funcs->elements[0].locals.elements, nullptr) << "Expected first function locals to be null";
    ASSERT_EQ(funcs->elements[0].body.instr, nullptr) << "Expected first function body instructions to be null";
    ASSERT_EQ(funcs->elements[0].body.end, nullptr) << "Expected first function body end to be null";
    // Clean up
    free(mod.was); // Free the allocated memory for the decoded module
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
TEST(WASPC_VALIDATION_DECODER, DECODE_GLOBAL_SECTION_1){
    WpModuleState mod;
    WpModuleInit(&mod); // Initialize the module state

    mod.buf = (const uint8_t *)test1; // Set the buffer to the test WASM file
    mod.bufsize = sizeof(test1); // Set the buffer size to the size of the test WASM file
    
    
    ASSERT_NE(mod.was, nullptr) << "Failed to allocate memory for WasModule";
    mod.was->funcs.elements = nullptr; // Initialize the funcs vector to nullptr
    mod.was->funcs.lenght = 0; // Initialize the length of the funcs vector to 0

    const uint8_t *buf_end = mod.buf + mod.bufsize; // Pointer to the end of the buffer
    const uint8_t *index = mod.buf; 
    uint8_t section_id;

    index = index + 8; // Skip the magic number and version (8 bytes)
        
    
    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    section_id = READ_BYTE(); // Read the section ID index 0x08
    ASSERT_EQ(section_id, WP_WSA_BIN_MOD_SEC_ID_TYPE) << "Expected section ID to be TYPE";
    
    index = ReadBinSection(index, &mod.typesec);
    ASSERT_NE(index, nullptr) << "Failed to read type section";
    
    section_id = READ_BYTE(); // Read the next section ID 0x10
    ASSERT_EQ(section_id, WP_WSA_BIN_MOD_SEC_ID_FUNCTION) << "Expected section ID to be FUNCTION";
    // Decode the function section      
    index = ReadBinSection(index, &mod.functionsec);
    ASSERT_NE(index, nullptr) << "Failed to read function section";

    section_id = READ_BYTE(); // Read the next section ID index 0x14
    ASSERT_EQ(section_id, WP_WSA_BIN_MOD_SEC_ID_GLOBAL) << "Expected section ID to be GLOBAL";
    // Decode the global section ID to be GLOBAL";
    index = ReadBinSection(index, &mod.globalsec);  
    ASSERT_NE(index, nullptr) << "Failed to read global section";
    VecGlobal *globals = DecodeGlobalSection(&mod);
    ASSERT_NE(globals, nullptr) << "Failed to decode global section";
    ASSERT_EQ(globals->lenght, 1) << "Expected 1 global variable in the global section";
    ASSERT_NE(globals->elements, nullptr) << "Global elements should not be null";
    ASSERT_EQ(globals->elements[0].gt.t, WAS_I32) << "Expected first global value type to be I32";
    ASSERT_EQ(globals->elements[0].gt.m, 0) << "Expected first global mutability to be constant";
    ASSERT_EQ(*globals->elements[0].e.instr, 0x41) << "Expected first global expression instructions to be 0x41 (I32.const)";
    ASSERT_EQ(*globals->elements[0].e.end, 0x0B) << "Expected first global expression end to be 0x0B (end of expression)";
    // Clean up
    free(mod.was); // Free the allocated memory for the decoded module
    #undef READ_BYTE
    #undef NOT_END
}


TEST(WASPC_VALIDATION_DECODER, DECODE_EXPORT_SECTION_1){

    WpModuleState mod;
    WpModuleInit(&mod); // Initialize the module state

    mod.buf = (const uint8_t *)test1; // Set the buffer to the test WASM file
    mod.bufsize = sizeof(test1); // Set the buffer size to the size of the test WASM file
  
   
    ASSERT_NE(mod.was, nullptr) << "Failed to allocate memory for WasModule";
    mod.was->funcs.elements = nullptr; // Initialize the funcs vector to nullptr
    mod.was->funcs.lenght = 0; // Initialize the length of the funcs vector to 0

    const uint8_t *buf_end = mod.buf + mod.bufsize; // Pointer to the end of the buffer
    const uint8_t *index = mod.buf; 
    uint8_t section_id;

    index = index + 8; // Skip the magic number and version (8 bytes)
        
    
    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    section_id = READ_BYTE(); // Read the section ID index 0x08
    ASSERT_EQ(section_id, WP_WSA_BIN_MOD_SEC_ID_TYPE) << "Expected section ID to be TYPE";    
    index = ReadBinSection(index, &mod.typesec);
    ASSERT_NE(index, nullptr) << "Failed to read type section";
    
    section_id = READ_BYTE(); // Read the next section ID 0x10
    ASSERT_EQ(section_id, WP_WSA_BIN_MOD_SEC_ID_FUNCTION) << "Expected section ID to be FUNCTION";
    index = ReadBinSection(index, &mod.functionsec);
    ASSERT_NE(index, nullptr) << "Failed to read function section";

    section_id = READ_BYTE(); // Read the next section ID index 0x14
    ASSERT_EQ(section_id, WP_WSA_BIN_MOD_SEC_ID_GLOBAL) << "Failed to read global section";
    index = ReadBinSection(index, &mod.globalsec);
    ASSERT_NE(index, nullptr) << "Failed to read global section";

    //Decode Export section ID to be EXPORT
    section_id = READ_BYTE(); // Read the next section ID index 0x1C
    ASSERT_EQ(section_id, WP_WSA_BIN_MOD_SEC_ID_EXPORT) << "Expected section ID to be EXPORT";
    index = ReadBinSection(index, &mod.exportsec);
    ASSERT_NE(index, nullptr) << "Failed to read export section";
    VecExport *exports = DecodeExportSection(&mod);
    ASSERT_NE(exports, nullptr) << "Failed to decode export section";   
    ASSERT_EQ(exports->lenght, 1) << "Expected 1 export in the export section";
    ASSERT_NE(exports->elements, nullptr) << "Export elements should not be null";
    ASSERT_EQ(exports->elements[0].name.lenght, 4) << "Expected first export name length to be 4";
    ASSERT_STREQ(exports->elements[0].name.name, "main") << "Expected first export name to be 'main'";
    ASSERT_EQ(exports->elements[0].type, WP_EXTERNAL_TYPE_FUNC) << "Expected first export type to be function";
    ASSERT_EQ(exports->elements[0].desc.func, 0) << "Expected first export function index to be 0";   
    
    // Clean up
    free(mod.was); // Free the allocated memory for the decoded module
    #undef READ_BYTE
    #undef NOT_END
}

TEST(WASPC_VALIDATION_DECODER, DECODE_CODE_SECTION_1){

    WpModuleState mod;
    WpModuleInit(&mod); // Initialize the module state
    ASSERT_EQ(mod.status, WP_MODULE_STATUS_INIT) << "Module status should be INIT";
    ASSERT_NE(mod.was, nullptr) << "Failed to allocate memory for WasModule";
    ASSERT_EQ(mod.was->funcs.lenght, 0) << "Expected 0 function in init state";
    ASSERT_EQ(mod.was->funcs.elements, nullptr) << "Expected 0 function in init state";

    mod.buf = (const uint8_t *)test1; // Set the buffer to the test WASM file
    mod.bufsize = sizeof(test1); // Set the buffer size to the size of the test WASM file
    mod.was->funcs.elements = nullptr; // Initialize the funcs vector to nullptr
    mod.was->funcs.lenght = 0; // Initialize the length of the funcs vector to 0

    const uint8_t *buf_end = mod.buf + mod.bufsize; // Pointer to the end of the buffer
    const uint8_t *index = mod.buf; 
    uint8_t section_id;

    index = index + 8; // Skip the magic number and version (8 bytes)
        
    
    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    section_id = READ_BYTE(); // Read the section ID index 0x08
    ASSERT_EQ(section_id, WP_WSA_BIN_MOD_SEC_ID_TYPE) << "Expected section ID to be TYPE";    
    index = ReadBinSection(index, &mod.typesec);
    ASSERT_NE(index, nullptr) << "Failed to read type section";
    VecFuncType *types = DecodeTypeSection(&mod);
    
    section_id = READ_BYTE(); // Read the next section ID 0x10
    ASSERT_EQ(section_id, WP_WSA_BIN_MOD_SEC_ID_FUNCTION) << "Expected section ID to be FUNCTION";
    index = ReadBinSection(index, &mod.functionsec);
    ASSERT_NE(index, nullptr) << "Failed to read function section";
    VecFunc *funcs = DecodeFunctionSection(&mod);
    ASSERT_NE(funcs, nullptr) << "Failed to decode code section";
    ASSERT_EQ(funcs->lenght, 1) << "Expected 1 function in the code section";

    section_id = READ_BYTE(); // Read the next section ID index 0x14
    ASSERT_EQ(section_id, WP_WSA_BIN_MOD_SEC_ID_GLOBAL) << "Failed to read global section";
    index = ReadBinSection(index, &mod.globalsec);
    ASSERT_NE(index, nullptr) << "Failed to read global section";

    //Decode Export section ID to be EXPORT
    section_id = READ_BYTE(); // Read the next section ID index 0x1C
    ASSERT_EQ(section_id, WP_WSA_BIN_MOD_SEC_ID_EXPORT) << "Expected section ID to be EXPORT";
    index = ReadBinSection(index, &mod.exportsec);
    ASSERT_NE(index, nullptr) << "Failed to read export section";
    
    section_id = READ_BYTE(); // Read the next section ID index 0x26
    ASSERT_EQ(section_id, WP_WSA_BIN_MOD_SEC_ID_CODE) << "Expected section ID to be CODE";
    index = ReadBinSection(index, &mod.codesec);    
    ASSERT_NE(index, nullptr) << "Failed to read code section";
    funcs = DecodeCodeSection(&mod);  
    ASSERT_EQ(funcs->lenght, 1) << "Expected 1 function in the code section";
    ASSERT_NE(funcs->elements, nullptr) << "Function elements should not be null";
    //Checks locals  
    ASSERT_EQ(funcs->elements[0].locals.lenght, 1) << "Expected first function to have no local variables"; //0x2A
    ASSERT_NE(funcs->elements[0].locals.elements, nullptr) << "Expected first function locals to be null"; 
    ASSERT_EQ(funcs->elements[0].locals.elements[0].n, 1) << "Expected first function locals to have 1 local variable";
    ASSERT_EQ(funcs->elements[0].locals.elements[0].t, WAS_I32) << "Expected first function local variable type to be I32";       
    // Check if the function body is correctly decoded
    ASSERT_NE(funcs->elements[0].body.instr, nullptr) << "Expected first function body instructions to be not null";    
    ASSERT_NE(funcs->elements[0].body.end, nullptr) << "Expected final function body instructions to be not null"; 
    ASSERT_EQ(*funcs->elements[0].body.end, 0x0B) << "Expected final instruction of the function body to be end (0x0B)";    
    uint32_t body_size = funcs->elements[0].body.end - (funcs->elements[0].body.instr-1); // Calculate the size of the function body
    ASSERT_EQ(body_size, 19) << "Expected first function body size to be 19"; 
    ASSERT_EQ(funcs->elements[0].body.instr[0], 0x41) << "Expected first function body instruction to be I32.const (0x41)";
    ASSERT_EQ(funcs->elements[0].body.instr[1], 0x0C) << "Expected first function body instruction to be 0 (0x00)";
    ASSERT_EQ(funcs->elements[0].body.instr[2], 0x41) << "Expected first function body instruction to be end (0x0B)"; 
    ASSERT_EQ(*funcs->elements[0].body.end, 0x0B) << "Expected final instruction of the function body to be end (0x0B)"; // Check if the last instruction is end
    // Clean up
    free(mod.was); // Free the allocated memory for the decoded module
    #undef READ_BYTE
    #undef NOT_END
}