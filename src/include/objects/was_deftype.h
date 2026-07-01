/**
 * @file was_type.h
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-02-12
 * 
 * @copyright Copyright (c) 2025
 * 
 */

 #ifndef WASPC_OBJECTS_WAS_TYPE_H
 #define WASPC_OBJECTS_WAS_TYPE_H

#ifdef __cplusplus
    extern "C" {
#endif

//wasp includes
#include "objects/object.h"

#include <stdint.h>

typedef enum {
    WP_WAS_DEF_TYPE_FUNC_TYPE = 0x60,
    WP_WAS_DEF_TYPE_TABLE_TYPE,
    WP_WAS_DEF_TYPE_MEMORY_TYPE,
    WP_WAS_DEF_TYPE_GLOBAL_TYPE,
    WP_WAS_DEF_TYPE_IMPORT_TYPE,
    WP_WAS_DEF_TYPE_EXPORT_TYPE,
    WP_WAS_DEF_TYPE_TAG_TYPE,
} WpWasDefTypeEnum;

typedef struct WpWasDefFuncType {
    uint32_t param_len;
    const uint8_t *param_types;         ///pointer to binary file where the function type is defined, it can be used to compare with other function type without decode them into WasmValueType. It can also be used to get the function type when call indirect without decode the function type in table section.
    uint32_t ret_len;
    const uint8_t *ret_types;           ///pointer to binary file where the function type is defined, it can be used to compare with other function type without decode them into WasmValueType. It can also be used to get the function type when call indirect without decode the function type in table section.
} WpWasDefFuncType;

/**
 * @brief WpWasDefType represents a WebAssembly definition type.
 */
typedef struct WpWasDefType {
    /// head for all Waspc object to allow cast
    WpObjectType wp_type;
    struct WpWasDefType *next;

    WpWasDefTypeEnum def_type;
    const uint8_t *binary;    //pointer to the defined type in binary file, it can be used to compare with other defined type without decode them into WasmValueType. It can also be used to get the function type when call indirect without decode the function type in table section.
    //TODO implement the other recursive types.
   union {
        WpWasDefFuncType func_type;
        // Add other type definitions as needed
    } def;
    
}WpWasDefType;

// Methods **************************************************************************************************

void WpWasDefTypeInit(WpWasDefType *self, WpWasDefTypeEnum def_type);

#ifdef __cplusplus
    }
#endif

#endif