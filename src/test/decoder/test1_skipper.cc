#include <gtest/gtest.h>
#include "decoder/leb128.h"
#include "decoder/wasm_decoder.h"
#include "../wasm/file_reader.h"

#include <stdlib.h>

#define LOAD_WASM_FILE(filename, index, buf, len, buf_end)       \
    std::vector<uint8_t> index;                                  \
    std::string error;                                           \
    index.resize(waspc::test::wasm::ReadFileSize(filename));           \
    bool ok = waspc::test::wasm::ReadFileContent(filename, index.data(), error); \
    ASSERT_TRUE(ok) << "ReadFileContent falló: " << error;        \
    ASSERT_FALSE(index.empty());                                 \
    const uint8_t *buf = index.data();                           \
    uint32_t len = index.size();                                 \
    const uint8_t *buf_end = buf + len;


TEST(WASPC__DECODER, Function_SkipExprBuf) {    
    
    LOAD_WASM_FILE("sample1.wasm", index, buf, len, buf_end);
    //set index to init expresion for global 1
    buf = buf + 0x19; //OPCODE 0x41
    ASSERT_EQ(*buf, 0x41);
    buf = SkipExprBuf(buf, buf_end);
    ASSERT_EQ(*buf, 0x07); // next byte after 0x0B OPCODE
    buf = index.data() + 0x2D; // point to first OPCODE for function 1
    buf = SkipExprBuf(buf, buf_end);
   
}

/*
TEST(WASPC_DECODER, Function_SkipRecSubType){

    
}*/


TEST(WASPC_DECODER, Function_SkipLimitTypeBuf){

    LOAD_WASM_FILE("sample2.wasm", index, buf, len, buf_end);
}

/*
TEST(WASPC_DECODER, Function_SkipRefTypeBuf){
    
}



TEST(WASPC_DECODER, Function_SkipTableTypeBuf){

    
}

TEST(WASPC_DECODER, Function_SkipLocalBuf){

    
}

TEST(WASPC_DECODER, Function_SkipValTypeBuf){

    
}

TEST(WASPC_DECODER, Function_SkipMemArgBuf){

    
}
*/