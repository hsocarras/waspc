
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

/**
 * 
 */
typedef struct WasmBinSection {   
    /// Section size
    uint32_t size;
    /// Section body ( not include id and size)
    const uint8_t *content;
} WasmBinSection;

typedef struct WasmBinImport{
    uint32_t module_name_len;
    const uint8_t *module_name;
    uint32_t name_len;
    const uint8_t *name;
    uint8_t external_type;
    const uint8_t *external;    
} WasmBinImport;

/**
 * global ::= gt:globaltype 𝑒:expr
 * globaltype ::= 𝑡:valtype mut?:mut
 */
typedef struct WasmBinGlobal{
    uint8_t mut;
    const uint8_t *type;
    const uint8_t *init_expr;
} WasmBinGlobal;

/**
 * memtype ::= (at, lim):limits
 * limits ::= 0x00 𝑛:u64
 *          | 0x01 𝑛:u64 𝑚:u64
 *          | 0x04 𝑛:u64
 *          | 0x05 𝑛:u64 𝑚:u64
 */
typedef struct WasmBinMemory{
    uint8_t address_type;        //32 or 64
    uint64_t page_size_min;
    uint64_t page_size_max;
} WasmBinMemory;

/**
 * export ::= nm:name xx:externidx
 */
typedef struct WasmBinExport{
    uint32_t name_len;
    const uint8_t *name;    
    uint8_t index_type;
    uint32_t external_index;
}WasmBinExport;

/**
 * loc**:list(locals) 𝑒:expr
 * locals ::= 𝑛:u32 𝑡:valtype
 */
typedef struct WasmBinFunction{
    const uint8_t *locals;
    const uint8_t *body;
    const uint8_t *end;     //pointer to the end of the function body, used to check if the function body has been fully executed
} WasmBinFunction;

/**
 * 0x60 𝑡* 1:resulttype 𝑡* 2:resulttype
 */
typedef struct WasmBinFuncType{
    const uint8_t *type;
    uint32_t param_len;
    const uint8_t *param_types;
    uint32_t ret_len;
    const uint8_t *ret_types;
}WasmBinFuncType;

/**
 * memarg ::= 𝑛:u32 𝑚:u32
 *          | 𝑛:u32 𝑥:memidx 𝑚:u32
 */
typedef struct WasmBinMemArg{
    uint32_t n;                         //alignment expressed as a power of 2.
    uint32_t m;                         //offset expressed as an unsigned integer literal.
    uint32_t x;                         //memory index expressed as an unsigned integer literal.
} WasmBinMemArg;



#ifdef __cplusplus
    }
#endif

#endif // WASPC_WEBASSEMBLY_BIN_H