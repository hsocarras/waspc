#include "runtime/buildin/buildin.h"

static const uint8_t binariess[256] = { 
    [0] = 0x60, 0x01, 0x7C, 0x01, 0x7C,       //index 0 F64->F64
    [5] = 0x60, 0x01, 0x7D, 0x01, 0x7D,       //index 5 F32->F32
};

const WpWasDefType signatures[256] = { 
    [0] = {WP_OBJECT_DEFINED_TYPE, NULL, WP_WAS_DEF_TYPE_FUNC_TYPE, &binariess[0], {.func_type = {1, &binariess[1], 1, &binariess[3]}}}, // F64->F64
    [1] = {WP_OBJECT_DEFINED_TYPE, NULL, WP_WAS_DEF_TYPE_FUNC_TYPE, &binariess[5], {.func_type = {1, &binariess[6], 1, &binariess[8]}}}, // F32->F32
};
