#include <gtest/gtest.h>
#include "decoder/leb128.h"
#include "decoder/wasm_decoder.h"
#include "objects/module_state.h"
#include "objects/module_instance.h"
#include "../wasm/file_reader.h"


#include <stdlib.h>


static const uint8_t * ReadBinSection(const uint8_t *index, WasmBinSection *sec){

    uint32_t aux_u32;
    index = DecodeLeb128UInt32(index, &aux_u32);
    if (!index){
        return NULL;                                       
    }

    sec->size = aux_u32;
    sec->content = index;
    index = index + aux_u32;

    return index;

}


TEST(WASPC_VALIDATION_DECODER, GET_MEMORY_1){
    //read sample wasm file into a buffer
    std::vector<uint8_t> wasm_buffer;
    std::string error;
    wasm_buffer.resize(waspc::test::wasm::ReadFileSize("sample2.wasm"));
    bool ok = waspc::test::wasm::ReadFileContent("sample2.wasm", wasm_buffer.data(), error);
    ASSERT_TRUE(ok) << "ReadFileContent falló: " << error;
    ASSERT_FALSE(wasm_buffer.empty());

    const uint8_t *index  = wasm_buffer.data(); // Set the buffer to the test WASM file
    uint32_t len = wasm_buffer.size(); // Set the buffer size to the size of the test WASM file
    const uint8_t *buf_end = index + len; // Pointer to the end of the buffer    
    const uint8_t *functiontype_index;
    uint32_t functype_count;
    uint32_t typeidx;
    uint8_t section_id;
    WasmBinSection typesec;
    WasmBinSection functionsec;
    WasmBinSection memorysec;

    index = index + 8; // Skip the magic number and version (8 bytes)

    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    //type section
    section_id = READ_BYTE(); // Read the section ID index 0x08
    ASSERT_EQ(section_id, WP_WSA_BIN_MOD_SEC_ID_TYPE) << "Expected section ID to be TYPE";    
    index = ReadBinSection(index, &typesec);
    ASSERT_NE(index, nullptr) << "Failed to read type section";

    //function section
    section_id = READ_BYTE(); // Read the next section ID 0x10
    ASSERT_EQ(section_id, WP_WSA_BIN_MOD_SEC_ID_FUNCTION) << "Expected section ID to be FUNCTION";
    index = ReadBinSection(index, &functionsec);
    ASSERT_NE(index, nullptr) << "Failed to read function section";

    //memory section
    section_id = READ_BYTE(); // Read the next section ID index 0x17
    ASSERT_EQ(section_id, WP_WSA_BIN_MOD_SEC_ID_MEMORY) << "Expected section ID to be MEMORY";
    index = ReadBinSection(index, &memorysec);
    ASSERT_NE(index, nullptr) << "Failed to read memory section";
    ASSERT_EQ(memorysec.content - wasm_buffer.data(), 0x24) << "Expected memory section to start at byte 0x24";
    ASSERT_EQ(memorysec.size, 3) << "Expected memory section size to be 3 bytes";

    const uint8_t *mem_index = GetGlobalByIndex(memorysec, 0);
    ASSERT_NE(mem_index, nullptr) << "Failed to read memory at index 0";
    ASSERT_EQ(*mem_index, 0x00) << "Expected memory type to be 0x00 (indicating a memory with limits)";

    #undef READ_BYTE
    #undef NOT_END
    
}
