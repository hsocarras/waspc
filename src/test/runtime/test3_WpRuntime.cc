#include <gtest/gtest.h>
#include "objects/module_state.h"
#include "objects/module_instance.h"
#include "interpreter/values.h"
#include "runtime/runtime.h"
#include "../wasm/file_reader.h"

#include <stdlib.h>
#include <cstdio>

static uint8_t MEM[4096*64];
static uint8_t DATA_MEM[4096*8*128];
static StackValue val[256];
static CallFrame call_stack[64];
static HtModuleEntry table[10]; // Allocate memory for the hash table

TEST(WASPC_RUNTIME_RUNTIME, RUNTIME_EXECUTE_IMPORT_FUNCTION) {

    // Initialize the runtime /////////////////////////////////////////////////////////////////////////////
    WpRuntimeState runtime;
    WpRuntimeInit(&runtime);    
    WpRuntimeSetMemoryStore(&runtime, MEM, 4096*64);
    WpRuntimeSetMemoryData(&runtime, DATA_MEM, 4096*8*128);
    WpRuntimeSetMemoryValueStack(&runtime, val, 256); 
    WpRuntimeSetMemoryCallStack(&runtime, call_stack, 64);  
    WpRuntimeSetMemoryHashTable(&runtime, table, 10); // Set the hash table memory and capacity 
    
    ASSERT_EQ(runtime.interpreter.call_stack_size, 64) << "Call stack size is not set correctly";
    ASSERT_EQ(runtime.interpreter.call_stack, call_stack) << "Call stack pointer is not set correctly";
    ////////////////////////////////////////////////////////////////////////////////////////////////////////

    //load sammple wasm file into a buffer
    std::vector<uint8_t> wasm_buffer;
    std::vector<uint8_t> wasm_buffer2;
    std::string error;
    bool ok = waspc::test::wasm::ReadFileContent("sample2.wasm", wasm_buffer, error);
    ASSERT_TRUE(ok) << "ReadFileContent falló: " << error;
    ASSERT_FALSE(wasm_buffer.empty());
    ///////////////////////////////////////////////////////////////////////////////////////////////////////    
   
    uint32_t len = wasm_buffer.size(); // Get the size of the test WASM file

    //Creating a binary file struct to pass to the function
    WpBinFile bin_file = {
        wasm_buffer.data(), // Allocate memory for the binary file buffer
        len     // Set the buffer size to the size of the test WASM file
    };

    WpModuleState *mod_state;
    
    WpObject *result = WpRuntimeCreateModuleFromBinFile(&runtime, bin_file, "points");
    if(result->wp_type == WP_OBJECT_ERROR) {
        WpError *error = (WpError *)result;
        FAIL() << "WpRuntimeCreateModuleFromBinFile returned an error: " << error->id;
    }
    if(result->wp_type != WP_OBJECT_MODULE_STATE) {
        FAIL() << "WpRuntimeCreateModuleFromBinFile did not return a WpModuleState object";
    }
    mod_state = (WpModuleState *)result;
        
    result = WpRuntimeInstanciateModule(&runtime, mod_state);
    // Check if the result is not null
    ASSERT_NE(result, nullptr) << "WpRuntimeInstantiateModule returned null";
    if(result->wp_type == WP_OBJECT_ERROR) {
        WpError *error = (WpError *)result;
        FAIL() << "WpRuntimeInstantiateModule returned an error: " << error->id;
    }
    // Check if the result is a module state object 
    ASSERT_EQ(result->wp_type, WP_OBJECT_MODULE_INSTANCE) << "WpRuntimeInstantiateModule did not return a WpModuleState object";
    ASSERT_NE(mod_state->instance, nullptr) << "Module instance is null after instantiation";
    ASSERT_EQ(mod_state->status, WP_MODULE_STATUS_INSTANTIATED) << "Module status is not set to INSTANTIATED after instantiation";
    ASSERT_EQ(mod_state->instance->wp_type, WP_OBJECT_MODULE_INSTANCE) << "Module instance wp_type is not WP_OBJECT_MODULE_INSTANCE";
    ASSERT_EQ(mod_state->instance->export_count, 3) << "Module instance export count is not 3";
    ASSERT_EQ(mod_state->instance->exports[0].export_type, 2) << "First export type is not 2 (memory)";
    ASSERT_EQ(mod_state->instance->exports[1].export_type, 0) << "Second export type is not 0 (function)";
    ASSERT_EQ(mod_state->instance->exports[2].export_type, 0) << "Third export type is not 0 (function)";
    ASSERT_EQ(mod_state->instance->exports[0].name_len, 6) << "First export name length is not 6";
    ASSERT_EQ(mod_state->instance->exports[0].name[0], 'm') << "First export name is not 'memory'";
    ASSERT_EQ(mod_state->instance->exports[1].name_len, 3) << "Second export name length is not 3";
    ASSERT_EQ(mod_state->instance->exports[1].name[0], 's') << "Second export name is not 'get'";
    ASSERT_EQ(mod_state->instance->exports[2].name_len, 3) << "Third export name length is not 3";
    ASSERT_EQ(mod_state->instance->exports[2].name[0], 'g') << "Third export name is not 'set'";
    ////////////////////////////////////////////////////////////////////////////////////////////////////////
    
    //load sammple wasm file into a buffer
    ok = waspc::test::wasm::ReadFileContent("sample3.wasm", wasm_buffer2, error);
    ASSERT_TRUE(ok) << "ReadFileContent falló: " << error;
    ASSERT_FALSE(wasm_buffer.empty());
    
    WpBinFile bin_file2 = {
        wasm_buffer2.data(), // Allocate memory for the binary file buffer
        len     // Set the buffer size to the size of the test WASM file
    };
    ///////////////////////////////////////////////////////////////////////////////////////////////////////    
        
    result = WpRuntimeCreateModuleFromBinFile(&runtime, bin_file2, "main");
    if(result->wp_type == WP_OBJECT_ERROR) {
        WpError *error = (WpError *)result;
        FAIL() << "WpRuntimeCreateModuleFromBinFile returned an error: " << error->id;
    }
    if(result->wp_type != WP_OBJECT_MODULE_STATE) {
        FAIL() << "WpRuntimeCreateModuleFromBinFile did not return a WpModuleState object";
    }
    mod_state = (WpModuleState *)result;
    
    result = WpRuntimeInstanciateModule(&runtime, mod_state);
    // Check if the result is not null
    ASSERT_NE(result, nullptr) << "WpRuntimeInstantiateModule returned null";
    if(result->wp_type == WP_OBJECT_ERROR) {
        WpError *error = (WpError *)result;
        FAIL() << "WpRuntimeInstantiateModule returned an error: " << error->id;
    }
    // Check if the result is a module state object 
    ASSERT_EQ(result->wp_type, WP_OBJECT_MODULE_INSTANCE) << "WpRuntimeInstantiateModule did not return a WpModuleState object";
    WpModuleInstance *instantiated_module2 = (WpModuleInstance *)result;

    char func_name[5] = "main";
    StackValue args[3];    
    result = WpRuntimeInvokeFunction(&runtime, instantiated_module2, func_name, args, 0);
    ASSERT_NE(result, nullptr) << "WpRuntimeInvokeFunction returned null";
    if(result->wp_type == WP_OBJECT_ERROR) {
        WpError *error = (WpError *)result;
        FAIL() << "WpRuntimeCreateModuleFromBinFile returned an error: " << error->id;
    }
    ASSERT_EQ(((WpResult *)result)->len, 3) << "WpRuntimeInvokeFunction did not return the expected number of results";
    
}

