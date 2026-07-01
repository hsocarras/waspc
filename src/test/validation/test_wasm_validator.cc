#include <gtest/gtest.h>
#include "objects/module_state.h"
#include "objects/module_instance.h"
#include "validator/wasm_validator.h"
#include "validator/wasm_validator_private.h"
#include "decoder/wasm_decoder.h"
#include "../wasm/file_reader.h"


#include <stdlib.h>

TEST(WASPC_VALIDATION_VALIDATOR, VALIDATE_SECTION_BY_ID) {    
    //load sammple wasm file into a buffer
    std::vector<uint8_t> wasm_buffer;
    std::string error;
    bool ok = waspc::test::wasm::ReadFileContent("sample1.wasm", wasm_buffer, error);
    ASSERT_TRUE(ok) << "ReadFileContent falló: " << error;
    ASSERT_FALSE(wasm_buffer.empty());

    // Setup runtime and module state for this test only
    WpValidatorState validator;
    WpValidatorStateInit(&validator); // Initialize the validator state    
    WpError err;
    StackValue val[256];
    WpErrorInit(&err);
    validator.err = &err; // Set the error object for the validator
    validator.value_stack = val;
    validator.value_stack_top = val;
    validator.value_stack_end = val + 256;

    WpModuleState mod;
    WpModuleStateInit(&mod); // Initialize the module state
    mod.buf = wasm_buffer.data();
    mod.bufsize = wasm_buffer.size();
   
    WpObject *ret = WpValidatorValidateModule(&validator, &mod);    
    ASSERT_EQ(ret->wp_type, WP_OBJECT_MODULE_STATE) << "error validating module: expected a WpModuleState object " << ((WpError *)ret)->id;
    
    

    #undef READ_BYTE
   
}
