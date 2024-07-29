// Must include the gtest header to use the testing library
#include <vm/leb128.h>

#include <gtest/gtest.h>

#include <stdint.h>



// All tests must live within TEST* blocks
// Inside of the TEST block is a standard C++ scope
// TestTopic defines a topic of our test, e.g. NameOfFunctionTest
// TrivialEquality represents the name of this particular test
// It should be descriptive and readable to the user
// TEST is a macro, i.e., preprocessor replaces it with some code
TEST(AOS_VM_LEB128, Decoding_32_Integer) {
  
    uint8_t number1[6] = {0x21, 0x36, 0xCD, 0x04, 0xAA, 0xDF};     //encoded i32 589 at index 2. 2 Bytes
    uint8_t number2[6] = {0xD9, 0xE8, 0x9D, 0x01, 0xAA, 0xDF};     //encoded i32 2585689 at index 0. 4 Bytes
    uint8_t number3[6] = {0xD9, 0xD8, 0xD9, 0x51, 0xAA, 0xDF};     //encoded i32 -758568 at index 1. 3 Bytes

    uint8_t *buffer;
    DEC_INT32_LEB128 result;

    //First check
    buffer = &number1[2];
    result = DecodeLeb128Int32(buffer);
    EXPECT_EQ(result.len, 2);
    EXPECT_EQ(result.value, 589);

    //Second check
    buffer = &number2[0];
    result = DecodeLeb128Int32(buffer);
    EXPECT_EQ(result.len, 4);
    EXPECT_EQ(result.value, 2585689);

    //Third check
    buffer = &number3[1];
    result = DecodeLeb128Int32(buffer);
    EXPECT_EQ(result.len, 3);
    EXPECT_EQ(result.value, -758568);
}

TEST(AOS_VM_LEB128, Decoding_32_UInteger) {
  
    uint8_t number1[6] = {0x21, 0x36, 0xCD, 0x17, 0xAA, 0xDF};     //encoded u32 23 at index 3. 1 Bytes
    uint8_t number2[6] = {0xD9, 0xC8, 0x07, 0x01, 0xAA, 0xDF};     //encoded u32 968 at index 1. 2 Bytes
    uint8_t number3[6] = {0xD9, 0xD3, 0x88, 0xE5, 0xA5, 0x06};     //encoded u32 1689863251 at index 1. 5 Bytes

    uint8_t *buffer;
    DEC_UINT32_LEB128 result;

    //First check
    buffer = &number1[3];
    result = DecodeLeb128UInt32(buffer);
    EXPECT_EQ(result.len, 1);
    EXPECT_EQ(result.value, 23);

    //Second check
    buffer = &number2[1];
    result = DecodeLeb128UInt32(buffer);
    EXPECT_EQ(result.len, 2);
    EXPECT_EQ(result.value, 968);

    //Third check
    buffer = &number3[1];
    result = DecodeLeb128UInt32(buffer);
    EXPECT_EQ(result.len, 5);
    EXPECT_EQ(result.value, 1689863251);
}
