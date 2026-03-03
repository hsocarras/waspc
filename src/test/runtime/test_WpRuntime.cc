#include <gtest/gtest.h>
#include "config_test.h"
#include "utils/names.h"
#include "utils/hash_table.h"
#include "objects/module.h"
#include "../wasm/samples.h"

#include <stdlib.h>

using namespace waspc::test::runtime;


TEST(WASPC_RUNTIME_RUNTIME, RUNTIME_INIT) {
    
    WpRuntimeState runtime;
    WpRuntimeInit(&runtime);

    // Check if the error object is initialized correctly
    ASSERT_EQ(runtime.err.type, WP_OBJECT_ERROR) << "Error object type is not WP_OBJECT_ERROR";
    ASSERT_EQ(runtime.err.id, 0) << "Error object id is not initialized to 0";      

   
}

TEST(WASPC_RUNTIME_RUNTIME, RUNTIME_CREATE_MODULE_FROM_BIN_FILE) {
    WpRuntimeState runtime;
    WpRuntimeInit(&runtime);

    Name mod_name;
    char name[6] = "main1"; 
    mod_name.name = name;
    mod_name.lenght = 6;

    uint32_t len = sizeof(sample1); // Get the size of the test WASM file

    //Creating a binary file struct to pass to the function
    WpBinFile bin_file = {
        sample1, // Allocate memory for the binary file buffer
        len     // Set the buffer size to the size of the test WASM file
    };

    WpModuleState mod_state;
    WpModuleInit(&mod_state);

    WpObject *result = WpRuntimeCreateModuleFromBinFile(&runtime, &mod_state, bin_file, mod_name);
    ASSERT_NE(result, nullptr) << "WpRuntimeCreateModuleFromBinFile returned null";
    ASSERT_EQ(result->type, WP_OBJECT_MODULE_STATE) << "WpRuntimeCreateModuleFromBinFile did not return a WpModuleState object";
}

TEST(WASPC_RUNTIME_RUNTIME, RUNTIME_VALIDATE_MODULE) {
    WpRuntimeState runtime;
    WpRuntimeInit(&runtime);

    Name mod_name;
    char name[6] = "main1"; 
    mod_name.name = name;
    mod_name.lenght = 6;

    uint32_t len = sizeof(sample1); // Get the size of the test WASM file

    //Creating a binary file struct to pass to the function
    WpBinFile bin_file = {
        &sample1[0], // Allocate memory for the binary file buffer
        len     // Set the buffer size to the size of the test WASM file
    };

    WpModuleState mod_state;
    WpModuleInit(&mod_state);

    WpObject *result = WpRuntimeCreateModuleFromBinFile(&runtime, &mod_state, bin_file, mod_name);
    
    result = WpRuntimeValidateModule(&runtime, &mod_state);
    // Check if the result is not null
    ASSERT_NE(result, nullptr) << "WpRuntimeValidateModule returned null";
    if(result->type == WP_OBJECT_ERROR) {
        WpError *error = (WpError *)result;
        FAIL() << "WpRuntimeValidateModule returned an error: " << error->id;
    }
    // Check if the result is a module state object 
    ASSERT_EQ(result->type, WP_OBJECT_MODULE_STATE) << "WpRuntimeValidateModule did not return a WpModuleState object";
    WpModuleState *validated_module = (WpModuleState *)result;
    // Check if the module status is validated
    ASSERT_EQ(validated_module->status, WP_MODULE_STATUS_VALIDATED) << "Module status is not WP_MODULE_STATUS_VALIDATED";
   
}
