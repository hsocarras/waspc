#include <gtest/gtest.h>
#include "config_test.h"
#include "utils/names.h"
#include "utils/hash_table.h"
#include "objects/module.h"
#include "../wasm/samples.h"


#include <stdlib.h>

using namespace waspc::test::validation;

TEST(WASPC_VALIDATION_VALIDATOR, VALIDATE_SECTION_BY_ID) {    
    
    Name mod_name;
    char name[6] = "main1";
    mod_name.name = name;
    mod_name.lenght = 6;

    // Setup runtime and module state for this test only
    WpRuntimeState runtime;
    WpRuntimeInit(&runtime);

    WpModuleState mod;
    WpModuleInit(&mod); // Initialize the module state
    const uint8_t *index  = &sample1[0]; // Set the buffer to the test WASM file
    uint32_t len = sizeof(sample1); // Set the buffer size to the size of the test WASM file
    const uint8_t *buf_end = index + len;
    uint8_t section_id;
    uint8_t last_loaded_section = 0; // Variable to keep track of section order.
    
    #define READ_BYTE() (*index++)

    index = index + 8; // Skip the magic number and version (4 bytes each)

    //TYPE SECTION///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    section_id = READ_BYTE();
    ASSERT_EQ(section_id, WP_WSA_BIN_MOD_SEC_ID_TYPE) << "Expected section ID to be TYPE";
    index = ValidateBinSectionById(&runtime.validator, index, section_id, &last_loaded_section, &mod);
    ASSERT_NE(index, nullptr) << "ValidateBinSectionById returned null for section ID: " << (int)section_id << " with error id: " << runtime.validator.err.id;
    // Check if the section was processed correctly   
    ASSERT_EQ(last_loaded_section, WP_WSA_BIN_MOD_SEC_ID_TYPE) << "Last loaded section should be TYPE";
    
    //FUNCTION SECTION/////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    section_id = READ_BYTE();
    ASSERT_EQ(section_id, WP_WSA_BIN_MOD_SEC_ID_FUNCTION) << "Expected section ID to be FUNCTION";
    index = ValidateBinSectionById(&runtime.validator, index, section_id, &last_loaded_section, &mod);
    ASSERT_NE(index, nullptr) << "ValidateBinSectionById returned null for section ID: " << (int)section_id;
    ASSERT_NE(mod.status, WP_MODULE_STATUS_INVALID) << "Module status should not be invalid after function section validation";
    ASSERT_NE(mod.status, WP_MODULE_STATUS_ERROR) << "Module status should not be Error after function section validation";
    // Check if the section was processed correctly 
    ASSERT_EQ(last_loaded_section, WP_WSA_BIN_MOD_SEC_ID_FUNCTION) << "Last loaded section should be FUNCTION";
    
    //GLOBAL SECTION///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    section_id = READ_BYTE();
    ASSERT_EQ(section_id, WP_WSA_BIN_MOD_SEC_ID_GLOBAL) << "Expected section ID to be GLOBAL";
    index = ValidateBinSectionById(&runtime.validator, index, section_id, &last_loaded_section, &mod);
    ASSERT_NE(index, nullptr) << "ValidateBinSectionById returned null for section ID: " << (int)section_id;
    // Check if the section was processed correctly     
    ASSERT_EQ(last_loaded_section, WP_WSA_BIN_MOD_SEC_ID_GLOBAL) << "Last loaded section should be GLOBAL";    
    ASSERT_NE(mod.status, WP_MODULE_STATUS_INVALID) << "Module status should not be invalid after global section validation";
    ASSERT_NE(mod.status, WP_MODULE_STATUS_ERROR) << "Module status should not be Error after global section validation";

    //EXPORT SECTION///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    section_id = READ_BYTE();
    ASSERT_EQ(section_id, WP_WSA_BIN_MOD_SEC_ID_EXPORT) << "Expected section ID to be EXPORT";
    index = ValidateBinSectionById(&runtime.validator, index, section_id, &last_loaded_section, &mod);
    ASSERT_NE(index, nullptr) << "ValidateBinSectionById returned null for section ID: " << (int)section_id;
    // Check if the section was processed correctly
    ASSERT_EQ(last_loaded_section, WP_WSA_BIN_MOD_SEC_ID_EXPORT) << "Last loaded section should be EXPORT"; 

    //CODE SECTION////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    section_id = READ_BYTE();
    ASSERT_EQ(section_id, WP_WSA_BIN_MOD_SEC_ID_CODE) << "Expected section ID to be CODE";
    index = ValidateBinSectionById(&runtime.validator, index, section_id, &last_loaded_section, &mod);
    ASSERT_NE(index, nullptr) << "ValidateBinSectionById returned null for section ID: " << (int)section_id;    
    // Check if the section was processed correctly
    ASSERT_EQ(last_loaded_section, WP_WSA_BIN_MOD_SEC_ID_CODE) << "Last loaded section should be CODE";
   

    #undef READ_BYTE
   
}
