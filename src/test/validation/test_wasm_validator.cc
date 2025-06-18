#include <gtest/gtest.h>
#include "config_test.h"
#include "utils/names.h"
#include "utils/hash_table.h"

#include <stdlib.h>

using namespace waspc::test::validation;
const char *test1_wasm = "C:\\wasm\\test1.wasm";
 
WpRuntimeState runtime;
HtEntry table[20]; // Allocate memory for the hash table

static WpObject * GetModuleState(const char *path, Name name) {
    // Initialize the runtime state
    WpRuntimeInit(&runtime);
    WpRuntimeCodeMemInit(&runtime, work_code_mem, 65536); 
    // Initialize the hash table for modules
    WpRuntimetableInit(&runtime, table, 10);  // Initialize the hash table with 10 capacity
    // Opening file in reading mode/////////////////////////////////////////////////////////////////////////
    FILE *wasm;   
    errno_t error_code;                             //Declaring a variable of type errno_t to store the return value of fopen_s
    error_code = fopen_s(&wasm, path, "rb");
    if (error_code != 0) {
        return nullptr; // Return null if file opening fails
    }

    // Positioning cursor at file end
    fseek( wasm , 0L , SEEK_END);
    // Getting file's size
    int32_t len;
    len = ftell(wasm);
    // Positioning cursor at begining
    rewind(wasm); 
    //Reading file into runtime code memory
    uint8_t *load_ptr = (uint8_t *)malloc(len);              //Declaring a variable to store the wasm file    
    if (!load_ptr) {
        return nullptr; // Return null if memory allocation fails
    }
    uint32_t bytes_read;
    bytes_read = fread(load_ptr, 1, len, wasm);
    // READ module from file into runtime memory
    return WpRuntimeReadModule(&runtime, name, load_ptr, len);  
}

TEST(WASPC_VALIDATION_VALIDATOR, VALIDATE_SECTION_BY_ID) {    
    
    Name mod_name;
    char name[6] = "main1";
    mod_name.name = name;
    mod_name.lenght = 6;

    // READ module from file into runtime memory
    WpObject *result = GetModuleState(test1_wasm, mod_name);     
    ASSERT_NE(result, nullptr) << "WpRuntimeReadModule returned null";
    ASSERT_EQ(result->type, WP_OBJECT_MODULE_STATE) << "WpRuntimeReadModule did not return a WpModuleState object";
    WpModuleState *mod = (WpModuleState *)result;
    // allocate memory for decoded module
    mod->was = (WasModule *)malloc(sizeof(WasModule));

    const uint8_t *index = mod->buf; 
    uint8_t section_id;
    uint8_t last_loaded_section = 0; // Variable to keep track of section order.
    
    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    index = index + 8; // Skip the magic number and version (4 bytes each)
    section_id = READ_BYTE();
    index = ValidateBinSectionById(index, section_id, &last_loaded_section, mod);
    ASSERT_NE(index, nullptr) << "ValidateBinSectionById returned null for section ID: " << (int)section_id;
    // Check if the section was processed correctly   
    ASSERT_EQ(last_loaded_section, WP_WSA_BIN_MOD_SEC_ID_TYPE) << "Last loaded section should be TYPE";

    section_id = READ_BYTE();
    index = ValidateBinSectionById(index, section_id, &last_loaded_section, mod);
    ASSERT_NE(index, nullptr) << "ValidateBinSectionById returned null for section ID: " << (int)section_id;
    // Check if the section was processed correctly 
    ASSERT_EQ(last_loaded_section, WP_WSA_BIN_MOD_SEC_ID_FUNCTION) << "Last loaded section should be FUNCTION";
    /*
    index = ValidateBinSectionById(index, section_id, &last_loaded_section, mod);
    ASSERT_NE(index, nullptr) << "ValidateBinSectionById returned null for section ID: " << (int)section_id;
    // Check if the section was processed correctly 
    ASSERT_EQ(last_loaded_section, WP_WSA_BIN_MOD_SEC_ID_TABLE) << "Last loaded section should be TABLE";

    index = ValidateBinSectionById(index, section_id, &last_loaded_section, mod);
    ASSERT_NE(index, nullptr) << "ValidateBinSectionById returned null for section ID: " << (int)section_id;
    // Check if the section was processed correctly
    ASSERT_EQ(last_loaded_section, WP_WSA_BIN_MOD_SEC_ID_MEMORY) << "Last loaded section should be MEMORY";
    */
    section_id = READ_BYTE();
    index = ValidateBinSectionById(index, section_id, &last_loaded_section, mod);
    ASSERT_NE(index, nullptr) << "ValidateBinSectionById returned null for section ID: " << (int)section_id;
    // Check if the section was processed correctly     
    ASSERT_EQ(last_loaded_section, WP_WSA_BIN_MOD_SEC_ID_GLOBAL) << "Last loaded section should be GLOBAL";

    section_id = READ_BYTE();
    index = ValidateBinSectionById(index, section_id, &last_loaded_section, mod);
    ASSERT_NE(index, nullptr) << "ValidateBinSectionById returned null for section ID: " << (int)section_id;
    // Check if the section was processed correctly
    ASSERT_EQ(last_loaded_section, WP_WSA_BIN_MOD_SEC_ID_EXPORT) << "Last loaded section should be EXPORT"; 
    
    /*
    section_id = READ_BYTE();
    index = ValidateBinSectionById(index, section_id, &last_loaded_section, mod);
    ASSERT_NE(index, nullptr) << "ValidateBinSectionById returned null for section ID: " << (int)section_id;
    // Check if the section was processed correctly
    ASSERT_EQ(last_loaded_section, WP_WSA_BIN_MOD_SEC_ID_START) << "Last loaded section should be START";   
    
    index = ValidateBinSectionById(index, section_id, &last_loaded_section, mod);
    ASSERT_NE(index, nullptr) << "ValidateBinSectionById returned null for section ID: " << (int)section_id;
    // Check if the section was processed correctly
    ASSERT_EQ(last_loaded_section, WP_WSA_BIN_MOD_SEC_ID_ELEMENT) << "Last loaded section should be ELEMENT";
    index = ValidateBinSectionById(index, section_id, &last_loaded_section, mod);
    ASSERT_NE(index, nullptr) << "ValidateBinSectionById returned null for section ID: " << (int)section_id;    
    // Check if the section was processed correctly
    ASSERT_EQ(last_loaded_section, WP_WSA_BIN_MOD_SEC_ID_DATA_COUNT) << "Last loaded section should be DATA_COUNT";
    */
    section_id = READ_BYTE();
    index = ValidateBinSectionById(index, section_id, &last_loaded_section, mod);
    ASSERT_NE(index, nullptr) << "ValidateBinSectionById returned null for section ID: " << (int)section_id;    
    // Check if the section was processed correctly
    ASSERT_EQ(last_loaded_section, WP_WSA_BIN_MOD_SEC_ID_CODE) << "Last loaded section should be CODE";

    /*
    section_id = READ_BYTE();
    index = ValidateBinSectionById(index, section_id, &last_loaded_section, mod);
    ASSERT_NE(index, nullptr) << "ValidateBinSectionById returned null for section ID: " << (int)section_id;    
    // Check if the section was processed correctly
    ASSERT_EQ(last_loaded_section, WP_WSA_BIN_MOD_SEC_ID_DATA) << "Last loaded section should be DATA";
    */
   
    // Clean up
    free(mod->was); // Free the allocated memory for the decoded module

    #undef READ_BYTE
    #undef NOT_END
   
}


