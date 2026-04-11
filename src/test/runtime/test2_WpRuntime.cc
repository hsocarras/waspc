#include <gtest/gtest.h>
#include "objects/module.h"
#include "interpreter/values.h"
#include "runtime/runtime.h"
#include "../wasm/file_reader.h"

#include <stdlib.h>


static uint8_t MEM[4096*64];
static uint8_t DATA_MEM[4096*8*128];
static StackValue val[256];
static CallFrame call_stack[64];

TEST(WASPC_RUNTIME_RUNTIME, RUNTIME_EXECUTE_EXPORT_FUNCTION) {

    //load sammple wasm file into a buffer
    std::vector<uint8_t> wasm_buffer;
    std::string error;
    bool ok = waspc::test::wasm::ReadFileContent("sample2.wasm", wasm_buffer, error);
    ASSERT_TRUE(ok) << "ReadFileContent falló: " << error;
    ASSERT_FALSE(wasm_buffer.empty());
    
    // Initialize the runtime /////////////////////////////////////////////////////////////////////////////
    WpRuntimeState runtime;
    WpRuntimeInit(&runtime);    
    WpRuntimeSetMemoryStore(&runtime, MEM, 4096*64);
    WpRuntimeSetMemoryData(&runtime, DATA_MEM, 4096*8*128);
    WpRuntimeSetMemoryValueStack(&runtime, val, 256); 
    WpRuntimeSetMemoryCallStack(&runtime, call_stack, 64);    
    ASSERT_EQ(runtime.interpreter.call_stack_size, 64) << "Call stack size is not set correctly";
    ASSERT_EQ(runtime.interpreter.call_stack, call_stack) << "Call stack pointer is not set correctly";
    ///////////////////////////////////////////////////////////////////////////////////////////////////////    
   
    uint32_t len = wasm_buffer.size(); // Get the size of the test WASM file

    //Creating a binary file struct to pass to the function
    WpBinFile bin_file = {
        wasm_buffer.data(), // Allocate memory for the binary file buffer
        len     // Set the buffer size to the size of the test WASM file
    };

    WpModuleState mod_state;
    WpModuleInit(&mod_state);

    WpObject *result = WpRuntimeCreateModuleFromBinFile(&runtime, &mod_state, bin_file);

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
    ASSERT_EQ(instantiated_module->type_count, 3) << "Module type count is not 1";

    //Set args
    StackValue args[3];
    args[0].type = WAS_VAL_TYPE_I32;
    args[0].value.i32 = 0;
    args[1].type = WAS_VAL_TYPE_F64;
    args[1].value.f64 = 10.0;
    args[2].type = WAS_VAL_TYPE_F64;
    args[2].value.f64 = 15.0;

    char func_name[4] = "set";

    result = WpRuntimeInvokeFunction(&runtime, &mod_state, func_name, args, 3);
    ASSERT_NE(result, nullptr) << "WpRuntimeInstantiateModule returned null";
    if(result->wp_type == WP_OBJECT_ERROR) {
        WpError *error = (WpError *)result;
        FAIL() << "WpRuntimeInvokeFunction returned an error: " << error->id;
    }
    /*
    args[0].value.i32 = 1;
    args[1].value.f64 = 20.0;
    args[2].value.f64 = 25.0;

    result = WpRuntimeInvokeFunction(&runtime, &mod_state, func_name, args, 3);
    ASSERT_NE(result, nullptr) << "WpRuntimeInstantiateModule returned null";
    if(result->wp_type == WP_OBJECT_ERROR) {
        WpError *error = (WpError *)result;
        FAIL() << "WpRuntimeInstantiateModule returned an error: " << error->id;
    }
    */
    StackValue index;
    index.type = WAS_VAL_TYPE_I32;  
    index.value.i32 = 0;
    func_name[0] = 'g';
    result = WpRuntimeInvokeFunction(&runtime, &mod_state, func_name, &index, 1);
    ASSERT_NE(result, nullptr) << "WpRuntimeInstantiateModule returned null";
    if(result->wp_type == WP_OBJECT_ERROR) {
        WpError *error = (WpError *)result;
        FAIL() << "WpRuntimeInstantiateModule returned an error: " << error->id;
    }
    ASSERT_EQ(result->wp_type, WP_OBJECT_RESULT) << "WpRuntimeInvokeFunction did not return a WpObjectResult object";
    WpResult *res = (WpResult *)result;
    ASSERT_EQ(res->len, 2) << "WpRuntimeInvokeFunction did not return the expected number of results";
    
}

