// Must include the gtest header to use the testing library
#include <utils/leb128.h>

#include <gtest/gtest.h>

#include <stdint.h>



// All tests must live within TEST* blocks
// Inside of the TEST block is a standard C++ scope
// TestTopic defines a topic of our test, e.g. NameOfFunctionTest
// TrivialEquality represents the name of this particular test
// It should be descriptive and readable to the user
// TEST is a macro, i.e., preprocessor replaces it with some code
TEST(WASPC_UTILS_LEB128, Decoding_32_Integer) {
  
    uint8_t number1[6] = {0x21, 0x36, 0xCD, 0x04, 0xAA, 0xDF};     //encoded i32 589 at index 2. 2 Bytes
    uint8_t number2[6] = {0xD9, 0xE8, 0x9D, 0x01, 0xAA, 0xDF};     //encoded i32 2585689 at index 0. 4 Bytes
    uint8_t number3[6] = {0xD9, 0xD8, 0xD9, 0x51, 0xAA, 0xDF};     //encoded i32 -758568 at index 1. 3 Bytes
    uint8_t number4[4] = {0xD9, 0xD8, 0xD9, 0x51};                 //encoded i32 -758568 at index 1. 3 Bytes

    const uint8_t *start;
    const uint8_t *end;
    int32_t value;

    //First check
    start = &number1[2];    
    end = DecodeLeb128Int32(start, &value);   
    EXPECT_EQ(end-start, 2);
    EXPECT_EQ(value, 589);

    //Second check
    start = &number2[0];
    end = DecodeLeb128Int32(start, &value);
    EXPECT_EQ(end-start, 4);
    EXPECT_EQ(value, 2585689);

    //Third check
    start = &number3[1];
    end = DecodeLeb128Int32(start, &value);
    EXPECT_EQ(end-start, 3);
    EXPECT_EQ(value, -758568);

    //Forth check end one byte ahead
    start = &number3[1];
    end = DecodeLeb128Int32(start, &value);
    EXPECT_EQ(end-start, 3);
    EXPECT_EQ(value, -758568);
}

TEST(WASPC_UTILS_LEB128, Decoding_32_UInteger) {
  
    uint8_t number1[6] = {0x21, 0x36, 0xCD, 0x17, 0xAA, 0xDF};     //encoded u32 23 at index 3. 1 Bytes
    uint8_t number2[6] = {0xD9, 0xC8, 0x07, 0x01, 0xAA, 0xDF};     //encoded u32 968 at index 1. 2 Bytes
    uint8_t number3[6] = {0xD9, 0xD3, 0x88, 0xE5, 0xA5, 0x06};     //encoded u32 1689863251 at index 1. 5 Bytes

    const uint8_t *start;
    const uint8_t *end;
    uint32_t value;

    //First check
    start = &number1[3];
    end = DecodeLeb128UInt32(start, &value);
    EXPECT_EQ(end-start, 1);
    EXPECT_EQ(value, 23);

    //Second check
    start = &number2[1];
    end = DecodeLeb128UInt32(start, &value);
    EXPECT_EQ(end-start, 2);
    EXPECT_EQ(value, 968);

    //Third check
    start = &number3[1];
    end = DecodeLeb128UInt32(start, &value);
    EXPECT_EQ(end-start, 5);
    EXPECT_EQ(value, 1689863251);
}
