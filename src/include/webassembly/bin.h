
#ifndef WASPC_WEBASSEMBLY_BIN_H
#define WASPC_WEBASSEMBLY_BIN_H

#ifdef __cplusplus
    extern "C" {
#endif

#include <stdint.h>

typedef enum SectionId{
    WP_WSA_BIN_MOD_SEC_ID_CUSTOM,         /// They are intended to be used for debugging information or third-party extensions. 5.5.3
    WP_WSA_BIN_MOD_SEC_ID_TYPE,           /// It decodes into a vector of function types. Spec 5.5.4.
    WP_WSA_BIN_MOD_SEC_ID_IMPORT,
    WP_WSA_BIN_MOD_SEC_ID_FUNCTION,
    WP_WSA_BIN_MOD_SEC_ID_TABLE,
    WP_WSA_BIN_MOD_SEC_ID_MEMORY,
    WP_WSA_BIN_MOD_SEC_ID_GLOBAL,
    WP_WSA_BIN_MOD_SEC_ID_EXPORT,
    WP_WSA_BIN_MOD_SEC_ID_START,
    WP_WSA_BIN_MOD_SEC_ID_ELEMENT,
    WP_WSA_BIN_MOD_SEC_ID_CODE,
    WP_WSA_BIN_MOD_SEC_ID_DATA,
    WP_WSA_BIN_MOD_SEC_ID_DATA_COUNT, 
    WP_WSA_BIN_MOD_SEC_ID_TAG    
} SectionId;


typedef struct WasmBinSection {   
    /// Section size
    uint32_t size;
    /// Section body ( not include id and size)
    const uint8_t *content;
} WasmBinSection;

typedef struct WasmBinGlobal{
    uint8_t mut;
    uint8_t type;
    const uint8_t *init_expr;
} WasmBinGlobal;

typedef struct WasmBinFunction{
    const uint8_t *locals;
    const uint8_t *body;
} WasmBinFunction;

#ifdef __cplusplus
    }
#endif

#endif // WASPC_WEBASSEMBLY_BIN_H