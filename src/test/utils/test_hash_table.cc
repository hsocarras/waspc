
#include "utils/hash_table.h"
#include <gtest/gtest.h>

#include <stdint.h>




TEST(WASPC_UTILS_HASH_TABLE, HASH_TABLE_SETUP) {
  
    HashTable ht;
    HtEntry entries[10];
    HashTableInit(&ht);
    HastTableSetup(&ht, entries, 10);

    EXPECT_EQ(ht.capacity, 10);
    EXPECT_EQ(ht.length, 0);
    EXPECT_NE(ht.entries, nullptr);

    for (int i = 0; i < 10; i++) {
        EXPECT_EQ(ht.entries[i].key.lenght, 0);
        EXPECT_EQ(ht.entries[i].key.name, nullptr);
        EXPECT_EQ(ht.entries[i].value, nullptr);
    }
}

TEST(WASPC_UTILS_HASH_TABLE, HASH_TABLE_SET_GET) {
  
    HashTable ht;
    HtEntry entries[10];
    HashTableInit(&ht);
    HastTableSetup(&ht, entries, 10);

    Name key1 = {4, "test"};
    int value1 = 42;
    HtEntry *result = HashTableSet(&ht, key1, &value1);
    
    EXPECT_EQ(result->value, &value1);
    EXPECT_EQ(ht.length, 1);

    void *retrieved_value = HashTableGet(&ht, key1);
    EXPECT_EQ(retrieved_value, &value1);

    Name key2 = {5, "hello"};
    int value2 = 100;
    result = HashTableSet(&ht, key2, &value2);
    
    EXPECT_EQ(*(int *)result->value, value2);
    EXPECT_EQ(ht.length, 2);

    retrieved_value = HashTableGet(&ht, key2);
    EXPECT_EQ(retrieved_value, &value2);
}
