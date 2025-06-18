
#ifndef WASPC_TEST_VALIDATION_CONFIG_TEST_H
#define WASPC_TEST_VALIDATION_CONFIG_TEST_H

// config_test.h
// Unit tests for runtime module
#include "runtime/runtime.h"

#include <gtest/gtest.h>



namespace waspc {
    namespace test {
        namespace validation {
            static uint8_t work_code_mem[65536];             
        } // namespace runtime    

    } // namespace test
} // namespace waspc

#endif // CONFIG_TEST_H