#include <gtest/gtest.h>
#include "config_test.h"
#include "utils/names.h"
#include "utils/hash_table.h"
#include "objects/module.h"
#include "../wasm/samples.h"
#include "webassembly/values.h"

#include <stdlib.h>

using namespace waspc::test::runtime;


TEST(WASPC_RUNTIME_RUNTIME, RUNTIME_INIT) {
    
    WpRuntimeState runtime;
    WpRuntimeInit(&runtime);

    // Check if the error object is initialized correctly
    ASSERT_EQ(runtime.err.wp_type, WP_OBJECT_ERROR) << "Error object type is not WP_OBJECT_ERROR";
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
    ASSERT_EQ(result->wp_type, WP_OBJECT_MODULE_STATE) << "WpRuntimeCreateModuleFromBinFile did not return a WpModuleState object";
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
    if(result->wp_type == WP_OBJECT_ERROR) {
        WpError *error = (WpError *)result;
        FAIL() << "WpRuntimeValidateModule returned an error: " << error->id;
    }
    // Check if the result is a module state object 
    ASSERT_EQ(result->wp_type, WP_OBJECT_MODULE_STATE) << "WpRuntimeValidateModule did not return a WpModuleState object";
    WpModuleState *validated_module = (WpModuleState *)result;
    // Check if the module status is validated
    ASSERT_EQ(validated_module->status, WP_MODULE_STATUS_VALIDATED) << "Module status is not WP_MODULE_STATUS_VALIDATED";
   
}

TEST(WASPC_RUNTIME_RUNTIME, RUNTIME_INSTANTIATE_MODULE) {

    // Initialize the runtime /////////////////////////////////////////////////////////////////////////////
    WpRuntimeState runtime;
    WpRuntimeInit(&runtime);
    uint8_t MEM[4096];
    StackValue val[256];
    runtime.store.mem = MEM;
    runtime.store.mem_size = 4096;
    runtime.store.mem_free = MEM;
    runtime.interpreter.value_stack = val;
    runtime.interpreter.value_stack_top = val;
    runtime.interpreter.value_stack_end = val + 256;
    ///////////////////////////////////////////////////////////////////////////////////////////////////////
    
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

    result = WpRuntimeInstanciateModule(&runtime, &mod_state, NULL, 0);
    // Check if the result is not null
    ASSERT_NE(result, nullptr) << "WpRuntimeInstantiateModule returned null";
    if(result->wp_type == WP_OBJECT_ERROR) {
        WpError *error = (WpError *)result;
        FAIL() << "WpRuntimeInstantiateModule returned an error: " << error->id;
    }
    // Check if the result is a module state object 
    ASSERT_EQ(result->wp_type, WP_OBJECT_MODULE_STATE) << "WpRuntimeInstantiateModule did not return a WpModuleState object";
    WpModuleState *instantiated_module = (WpModuleState *)result;
    // Check if the module status is instantiated
    ASSERT_EQ(instantiated_module->status, WP_MODULE_STATUS_VALIDATED) << "Module status is not WP_MODULE_STATUS_VALIDATED";
    ASSERT_EQ(instantiated_module->type_count, 1) << "Module type count is not 1";

    WpGlobalInstance *global1 = mod_state.globals;
    ASSERT_EQ(global1->mut, 0) << "Global 1 mutability is not 0";
    ASSERT_EQ(global1->type, 0x7f) << "Global 1 type is not i32";
    ASSERT_EQ(global1->val.value.i32, 25) << "Global 1 initial value is not 25";
   
    const WpFunctionInstance *func1 = mod_state.funcs;
    ASSERT_EQ(func1->wp_type, WP_OBJECT_FUNCTION_INSTANCE) << "Function 1 type is not WpFunctionInstance";
    //ASSERT_EQ(func1->func_type[0], mod_state.buf[11]) << "Function type is at index 0x0B";
    ASSERT_EQ(func1->locals[0], mod_state.buf[0x2B]) << "Locals start at index 0x2B";
    ASSERT_EQ(func1->body[0], mod_state.buf[0x2D]) << "Body start at index 0x2D";
}