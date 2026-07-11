#include <gtest/gtest.h>
#include "objects/module_state.h"
#include "objects/module_instance.h"
#include "interpreter/values.h"
#include "runtime/runtime.h"
#include "memory/load_memory.h"
#include "../wasm/file_reader.h"

#include <stdlib.h>

static uint8_t LOAD_MEMORY[4096];   //memory for binary files
static HtModuleEntry table[10];     // Allocate memory for the hash table
static uint8_t STORE[4096*64];
static uint8_t DATA_MEM[4096*8*128];
static StackValue val[256];
static CallFrame call_stack[64];

TEST(WASPC_RUNTIME_RUNTIME, RUNTIME_EXECUTE_EXPORT_FUNCTION) {
        
    // Initialize the runtime /////////////////////////////////////////////////////////////////////////////
    WpRuntimeState runtime;
    WpRuntimeInit(&runtime);    
    WpRuntimeSetMemoryStore(&runtime, STORE, 4096*64);
    WpRuntimeSetMemoryData(&runtime, DATA_MEM, 4096*8*128);
    WpRuntimeSetMemoryValueStack(&runtime, val, 256); 
    WpRuntimeSetMemoryCallStack(&runtime, call_stack, 64);  
    WpRuntimeSetLoadMemory(&runtime, LOAD_MEMORY, 4096, table, 10); // Set the hash table memory and capacity  
    
    ASSERT_EQ(runtime.interpreter.call_stack_size, 64) << "Call stack size is not set correctly";
    ASSERT_EQ(runtime.interpreter.call_stack, call_stack) << "Call stack pointer is not set correctly";
    ///////////////////////////////////////////////////////////////////////////////////////////////////////    

    //load sammple wasm file into a buffer
    WpBinFile bin_file;    
    uint32_t file_size = waspc::test::wasm::ReadFileSize("sample2.wasm");
    bin_file = WpLoadMemoryReserve(&runtime.modules, file_size);
    std::string error;
    bool ok = waspc::test::wasm::ReadFileContent("sample2.wasm", bin_file.buf, error);
    ASSERT_TRUE(ok) << "ReadFileContent falló: " << error;   

    ////////////////////////////////////////////////////////////////////////////////////////////////////////
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
    // Check if the module status is instantiated    
    ASSERT_EQ(instantiated_module->type_count, 3) << "Module type count is not 1";
    ASSERT_EQ(instantiated_module->export_count, 3) << "Module export count is not 3 SET, GET, and Memory";
    //Set args
    StackValue args[3];
    args[0].type = WAS_VAL_TYPE_I32;
    args[0].value.i32 = 0;
    args[1].type = WAS_VAL_TYPE_F64;
    args[1].value.f64 = 10.0;
    args[2].type = WAS_VAL_TYPE_F64;
    args[2].value.f64 = 15.0;

    char func_name[4] = "set";

    result = WpRuntimeInvokeFunction(&runtime, instantiated_module, func_name, args, 3);
    ASSERT_NE(result, nullptr) << "WpRuntimeInstantiateModule returned null";
    if(result->wp_type == WP_OBJECT_ERROR) {
        WpError *error = (WpError *)result;
        FAIL() << "WpRuntimeInvokeFunction returned an error: " << error->id;
    }
    
    args[0].value.i32 = 1;
    args[1].value.f64 = 20.0;
    args[2].value.f64 = 25.0;

    result = WpRuntimeInvokeFunction(&runtime, instantiated_module, func_name, args, 3);
    ASSERT_NE(result, nullptr) << "WpRuntimeInstantiateModule returned null";
    if(result->wp_type == WP_OBJECT_ERROR) {
        WpError *error = (WpError *)result;
        FAIL() << "WpRuntimeInstantiateModule returned an error: " << error->id;
    }
    
   
    StackValue index;
    index.type = WAS_VAL_TYPE_I32;  
    index.value.i32 = 0;
    func_name[0] = 'g';
    result = WpRuntimeInvokeFunction(&runtime, instantiated_module, func_name, &index, 1);
    ASSERT_NE(result, nullptr) << "WpRuntimeInstantiateModule returned null";
    if(result->wp_type == WP_OBJECT_ERROR) {
        WpError *error = (WpError *)result;
        FAIL() << "WpRuntimeInstantiateModule returned an error: " << error->id;
    }
    ASSERT_EQ(result->wp_type, WP_OBJECT_RESULT) << "WpRuntimeInvokeFunction did not return a WpObjectResult object";
    WpResult *res = (WpResult *)result;
    ASSERT_EQ(res->len, 2) << "WpRuntimeInvokeFunction did not return the expected number of results";
    
}

