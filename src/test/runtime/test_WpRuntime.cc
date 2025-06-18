#include <gtest/gtest.h>
#include "config_test.h"
#include "utils/names.h"
#include "utils/hash_table.h"

#include <stdlib.h>

using namespace waspc::test::runtime;



TEST(WASPC_RUNTIME_RUNTIME, RUNTIME_READ_MODULE) {
    // Initialize the runtime state
    WpRuntimeState runtime;
    WpRuntimeInit(&runtime);
    WpRuntimeCodeMemInit(&runtime, work_code_mem, 65536); 
    // Initialize the hash table for modules
    HtEntry table[10]; // Allocate memory for the hash table

    // Opening file in reading mode/////////////////////////////////////////////////////////////////////////
    FILE *wasm;   
    errno_t error_code;                             //Declaring a variable of type errno_t to store the return value of fopen_s
    
    error_code = fopen_s(&wasm, test1_wasm, "rb");
    if (error_code != 0) {
        FAIL() << "File can't be opened: " << test1_wasm;
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
        FAIL() << "Failed to allocate memory for loading wasm file.";
    }
    uint32_t bytes_read;
    bytes_read = fread(load_ptr, 1, len, wasm);
    Name mod_name;
    char name[6] = "main1";
    mod_name.name = name;
    mod_name.lenght = 6;

    // Case 1 table module capacity 0
    WpError *result = (WpError *)WpRuntimeReadModule(&runtime, mod_name, load_ptr, len); 
    // Check if the result is not null
    ASSERT_NE(result, nullptr) << "WpRuntimeReadModule returned null";  
    // Check if the result is an error object
    ASSERT_EQ(result->type, WP_OBJECT_ERROR) << "WpRuntimeReadModule did not return a WpModule object";
    ASSERT_EQ(result->id, 1) << "WpRuntimeReadModule did not return a WpModule object";

    // Case 2 table module capacity 0
    WpRuntimeInit(&runtime);
    WpRuntimeCodeMemInit(&runtime, work_code_mem, 65536);
    WpRuntimetableInit(&runtime, table, 10);  // Initialize the hash table with 10 capacity
    WpObject *result2 = WpRuntimeReadModule(&runtime, mod_name, load_ptr, len);    
    ASSERT_NE(result2, nullptr) << "WpRuntimeReadModule returned null";
    ASSERT_EQ(result2->type, WP_OBJECT_MODULE_STATE) << "WpRuntimeReadModule did not return a WpModuleState object";
    WpModuleState *module_state = (WpModuleState *)result2;
    // Check if the module name matches
    ASSERT_EQ(module_state->name.lenght, mod_name.lenght) << "Module name length does not match";
    ASSERT_EQ(strncmp(module_state->name.name, mod_name.name, mod_name.lenght), 0) << "Module name does not match";
    // Check if the module status is loaded
    ASSERT_EQ(module_state->status, WP_MODULE_STATUS_READ) << "Module status is not WP_MODULE_LOADED";
    // Check if the module buffer size matches the file size
    ASSERT_EQ(module_state->bufsize, len) << "Module buffer size does not match file size";    
    // Check if the module buffer is not null
    ASSERT_NE(module_state->buf, nullptr) << "Module buffer is null";
    // Check if the module buffer contains the correct data
    ASSERT_EQ(memcmp(module_state->buf, load_ptr, len), 0) << "Module buffer does not contain the correct data";

    fclose(wasm);
    free(load_ptr);
   
}

TEST(WASPC_RUNTIME_RUNTIME, RUNTIME_VALIDATE_MODULE) {
    // Initialize the runtime state
    WpRuntimeState runtime;
    WpRuntimeInit(&runtime);
    WpRuntimeCodeMemInit(&runtime, work_code_mem, 65536); 
    // Initialize the hash table for modules
    HtEntry table[10]; // Allocate memory for the hash table
    WpRuntimetableInit(&runtime, table, 10);  // Initialize the hash table with 10 capacity
    // Opening file in reading mode/////////////////////////////////////////////////////////////////////////
    FILE *wasm;   
    errno_t error_code;                             //Declaring a variable of type errno_t to store the return value of fopen_s
    
    error_code = fopen_s(&wasm, test1_wasm, "rb");
    if (error_code != 0) {
        FAIL() << "File can't be opened: " << test1_wasm;
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
        FAIL() << "Failed to allocate memory for loading wasm file.";
    }
    uint32_t bytes_read;
    bytes_read = fread(load_ptr, 1, len, wasm);
    Name mod_name;
    char name[6] = "main1";
    mod_name.name = name;
    mod_name.lenght = 6;

    // READ module from file into runtime memory
    WpObject *result = WpRuntimeReadModule(&runtime, mod_name, load_ptr, len);      
    ASSERT_NE(result, nullptr) << "WpRuntimeReadModule returned null";
    ASSERT_EQ(result->type, WP_OBJECT_MODULE_STATE) << "WpRuntimeReadModule did not return a WpModuleState object";
    WpModuleState *module_state = (WpModuleState *)result;

    result = WpRuntimeValidateModule(&runtime, module_state);
    // Check if the result is not null
    ASSERT_NE(result, nullptr) << "WpRuntimeValidateModule returned null";
    // Check if the result is a module state object 
    ASSERT_EQ(result->type, WP_OBJECT_MODULE_STATE) << "WpRuntimeValidateModule did not return a WpModuleState object";
    WpModuleState *validated_module = (WpModuleState *)result;
    // Check if the module status is validated
    ASSERT_EQ(validated_module->status, WP_MODULE_STATUS_VALIDATED) << "Module status is not WP_MODULE_STATUS_VALIDATED";    
   
}


