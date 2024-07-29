// Must include the gtest header to use the testing library
#include <vm/vm.h>

#include <gtest/gtest.h>

#include <stdint.h>



// All tests must live within TEST* blocks
// Inside of the TEST block is a standard C++ scope
// TestTopic defines a topic of our test, e.g. NameOfFunctionTest
// TrivialEquality represents the name of this particular test
// It should be descriptive and readable to the user
// TEST is a macro, i.e., preprocessor replaces it with some code
TEST(AOS_VM_LOADER, VALIDATE_MAGIC) {
  
    uint8_t number1[4] = {0x00, 0x61, 0x74, 0xD6};     
    uint8_t number2[4] = {0x00, 0x72, 0x73, 0xD6};     
    uint8_t number3[4] = {0x00, 0xD6, 0x73, 0x61};   
    uint8_t number4[4] = {0x00, 0x61, 0x73, 0x6D};  
    
    EXPECT_EQ(ValidateMagic(number1, 4), ERR_MAGIC);
    EXPECT_EQ(ValidateMagic(number2, 4), ERR_MAGIC);
    EXPECT_EQ(ValidateMagic(number3, 4), ERR_MAGIC);
    EXPECT_EQ(ValidateMagic(number4, 4), LOAD_OK);
}

TEST(AOS_VM_LOADER, VALIDATE_VERSION) {
  
    uint8_t number1[4] = {0x00, 0x00, 0x00, 0x01};     
    uint8_t number2[4] = {0x00, 0x01, 0x00, 0x00};     
    uint8_t number3[4] = {0x02, 0x00, 0x00, 0x00};   
    uint8_t number4[4] = {0x01, 0x00, 0x00, 0x00};  
    
    EXPECT_EQ(ValidateVersion(number1, 4), ERR_VERSION);
    EXPECT_EQ(ValidateVersion(number2, 4), ERR_VERSION);
    EXPECT_EQ(ValidateVersion(number3, 4), ERR_VERSION);
    EXPECT_EQ(ValidateVersion(number4, 4), LOAD_OK);
    
}

TEST(AOS_VM_LOADER, LOAD_VALIDATE_WASM) {
  
    EXPECT_EQ(1, 1);
}