#include <gtest/gtest.h>
#include "objects/module_state.h"
#include "objects/module_instance.h"
#include "interpreter/values.h"
#include "runtime/runtime.h"
#include "../wasm/file_reader.h"

#include <stdlib.h>


static uint8_t MEM[4096*64];
static uint8_t DATA_MEM[4096*8*128];
static StackValue val[256];
static CallFrame call_stack[64];
static HtModuleEntry table[10]; // Allocate memory for the hash table

TEST(WASPC_RUNTIME_RUNTIME, RUNTIME_EXECUTE_IMPORT_HOST_FUNCTION) {

    //load sammple wasm file into a buffer
    std::vector<uint8_t> wasm_buffer;
    std::string error;
    bool ok = waspc::test::wasm::ReadFileContent("sample4.wasm", wasm_buffer, error);
    ASSERT_TRUE(ok) << "ReadFileContent falló: " << error;
    ASSERT_FALSE(wasm_buffer.empty());
    
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
    ///////////////////////////////////////////////////////////////////////////////////////////////////////    
   
    uint32_t len = wasm_buffer.size(); // Get the size of the test WASM file

    //Creating a binary file struct to pass to the function
    WpBinFile bin_file = {
        wasm_buffer.data(), // Allocate memory for the binary file buffer
        len     // Set the buffer size to the size of the test WASM file
    };

    WpModuleState *mod_state;
    
    WpObject *result = WpRuntimeCreateModuleFromBinFile(&runtime, bin_file, "test_module");
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
    WpModuleInstance *instantiated_module = (WpModuleInstance *)result;  
    ASSERT_NE(instantiated_module->exports, nullptr) << "Module instance exports is null after instantiation";
    ASSERT_EQ(instantiated_module->function_count, 2) << "Module instance function count is not 2 after instantiation";
    ASSERT_EQ(instantiated_module->funcs[0].func_kind, WP_FUNC_HOST) << "Module instance first function is not a host function after instantiation";

    //Set args
    StackValue args[3];
    args[0].type = WAS_VAL_TYPE_F64;
    args[0].value.f64 = 0.4;
    args[1].type = WAS_VAL_TYPE_F64;
    args[1].value.f64 = 10.0;

    char func_name[9] = "calc_hyp";

    result = WpRuntimeInvokeFunction(&runtime, instantiated_module, func_name, args, 2);
    ASSERT_NE(result, nullptr) << "WpRuntimeInstantiateModule returned null";
    if(result->wp_type == WP_OBJECT_ERROR) {
        WpError *error = (WpError *)result;
        FAIL() << "WpRuntimeInvokeFunction returned an error: " << error->id;
    }
    ASSERT_EQ(result->wp_type, WP_OBJECT_RESULT) << "WpRuntimeInvokeFunction did not return a WpObjectResult object";
    WpResult *res = (WpResult *)result;
    ASSERT_EQ(res->len, 1) << "WpRuntimeInvokeFunction did not return the expected number of results";
    ASSERT_EQ(res->vals->type, WAS_VAL_TYPE_F64);
    ASSERT_NEAR(res->vals->value.f64, 25.679, 1e-3);

}

