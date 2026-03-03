/**
 * @file wasm_validator.c
 * @author your name (you@domain.com)
 * @brief
 * @version 0.1
 * @date 2024-08-21
 *
 * @copyright Copyright (c) 2024
 *
 */

#include "validation/wasm_validator.h"
#include "validation/wasm_decoder.h"
#include "webassembly/structure/module.h"
#include "webassembly/structure/types.h"
#include "webassembly/binary/module.h"
#include "webassembly/binary/instructions.h"
#include "utils/leb128.h"

#include <stdint.h>
// #include <stdio.h>

/**
 * @brief Inner function to read a binary section into a WasmBinSection structure
 *
 * @param index
 * @param sec
 * @return const uint8_t* index pointing at next section or NULL if error
 */
static const uint8_t *ReadBinSection(const uint8_t *index, WasmBinSection *sec)
{

    uint32_t aux_u32;
    index = DecodeLeb128UInt32(index, &aux_u32);
    if (!index)
    {
        return NULL;
    }

    sec->size = aux_u32;
    sec->content = index;
    index = index + aux_u32;

    return index;
}

/**
 * @brief Function to initialize the validator state.
 */
void WpValidatorStateInit(WpValidatorState *self)
{
    // Init error object
    WpErrorInit(&self->err);

    // Initialize the validator state context c
    // self->c.types.lenght = 0;
    // self->c.types.elements = NULL;
    // self->c.locals.lenght = 0;
    // self->c.locals.elements = NULL;
    // self->c.return_types.lenght = 0;
    // self->c.return_types.elements = NULL;

    // Initialize the prime context
    // self->c_prime.types.lenght = 0;
    // self->c_prime.types.elements = NULL;
    // self->c_prime.locals.lenght = 0;
    // self->c_prime.locals.elements = NULL;
    // self->c_prime.return_types.lenght = 0;
    // self->c_prime.return_types.elements = NULL;

    // self->val_stack = NULL;
    // self->stk_ptr = NULL;
    // self->val_stack_size = 0; // Set default stack size

    // self->ctrl_stack = NULL;
    // self->ctr_stack_idx = 0;  // Initialize control stack index
    // self->ctr_stack_size = 0; // Set default control stack size

    self->ip = NULL; // Initialize instruction pointer to NULL
}

/*TODO
uint8_t WpValidateInitExpr(WpValidatorState *self, const uint8_t *exp, uint32_t max_len, ValType t) {

    assert(exp != NULL);    // Ensure the expression pointer is not NULL

    const uint8_t *index = exp;                                                 // pointer to byte to traverse the binary file
    uint8_t opcode;                                                 // variable to store the opcode
    uint8_t result;
    uint32_t u32;
    int32_t i32;

    ActivationFrame frame = {exp, self->stk_ptr, 1, 0}; // Create a temporary control frame for the expression
    if(!WpValPushCtrlFrame(self, frame)){
        return 0; // Stack overflow error
    }

    #define READ_BYTE() (*frame.ip++)
    #define NOT_END() (frame.ip < exp + max_len)

    opcode = READ_BYTE(); // Read the next opcode
    while (opcode != 0x0B && NOT_END()) {

       //check constant opcode
        switch (opcode) {
            case OPCODE_I32_CONST:

                break;

            //case OPCODE_GLOBAL_GET:
                // Check if global type mut is constant
                //return 0;
            default:
                return 0; // Invalid opcode in constant expression
        }
    }

    // Check if the expression ended correctly
    if (*index != 0x0B) {
        return 0; // Expression did not end with 0x0B
    }

    return 1;

    #undef READ_BYTE
    #undef NOT_END
}
*/

/* TODO
 * @brief Function to validate a function in the validator state.
 * This function checks if the function type is valid, sets the local variables in the context.
 * It also validates the function body expression against the context.
 * @param self Pointer to the validator state
 * @param func Pointer to the function to validate
 * @return uint8_t 1 - ok, 0 - invalid
 *
uint8_t WpValidateFunction(WpValidatorState *self, const Func *func) {

    uint8_t result;

    //The type 𝐶.types[𝑥] must be defined in the context
    if (func->type_index >= self->c.types.lenght) {
        return 0; // Invalid function type index
    }

    // Setting c prime context
    self->c_prime.types.lenght = self->c.types.lenght;
    self->c_prime.types.elements = self->c.types.elements;
    //Get function type
    FuncType *ft = &self->c.types.elements[func->type_index];
    self->c_prime.locals.lenght = ft->params.lenght + func->locals.lenght;
    self->c_prime.locals.elements = self->stk_ptr;  // locals are stored in the value stack

    // Set the locals in the prime context
    for(uint32_t i = 0; i < ft->params.lenght; i++){
        result = WpValPushValType(self, ft->params.val_types[i]);
        if (!result) {
            return 0; // Stack overflow error
        }
    }
    for(uint32_t i = 0; i < func->locals.lenght; i++){
        result = WpValPushValType(self, func->locals.elements[i].t);
        if (!result) {
            return 0; // Stack overflow error
        }
    }
    // Set the return types in the prime context
    self->c_prime.return_types.lenght = ft->results.lenght;
    self->c_prime.return_types.elements = ft->results.val_types;

    //Under the context 𝐶′, the expression expr must be valid with type [𝑡* 2].
    if(!WpValidateExpr(self, &self->c_prime, &func->body)){
        return 0; // Invalid function body
    }

    return 1; // Function is valid
}*/

/**
 * @brief Function to decode and validate each section inside a module.
 * Checks if the section is in the correct order and validates binary sintax on each section.
 * @param self Pointer to the validator state
 * @param index
 * @param section_id
 * @param previous_secction
 * @param mod
 * @return const uint8_t* pointer to next section or NULL if error
 */
const uint8_t *ValidateBinSectionById(WpValidatorState *self, const uint8_t *index, const uint8_t section_id, uint8_t *previous_secction, WpModuleState *mod)
{

    uint32_t aux_u32; // auxiliary var to store u32 values
    uint32_t len;
    uint32_t i;
    uint8_t err_code;

    /**
     * @brief Sections inside a module are encoded in a specific order.
     * this switch case is to validate the order of the sections.
     */
    switch (*previous_secction)
    {
    case WP_WSA_BIN_MOD_SEC_ID_CUSTOM:
        goto start_at_type_sec;
        break;
    case WP_WSA_BIN_MOD_SEC_ID_TYPE:
        goto start_at_import_sec;
        break;
    case WP_WSA_BIN_MOD_SEC_ID_IMPORT:
        goto start_at_function_sec;
        break;
    case WP_WSA_BIN_MOD_SEC_ID_FUNCTION:
        goto start_at_table_sec;
        break;
    case WP_WSA_BIN_MOD_SEC_ID_TABLE:
        goto start_at_memory_sec;
        break;
    case WP_WSA_BIN_MOD_SEC_ID_MEMORY:
        goto start_at_tag_sec;
        break;
    case WP_WSA_BIN_MOD_SEC_ID_TAG:
        goto start_at_global_sec;
        break;
    case WP_WSA_BIN_MOD_SEC_ID_GLOBAL:
        goto start_at_export_sec;
        break;
    case WP_WSA_BIN_MOD_SEC_ID_EXPORT:
        goto start_at_start_sec;
        break;
    case WP_WSA_BIN_MOD_SEC_ID_START:
        goto start_at_element_sec;
        break;
    case WP_WSA_BIN_MOD_SEC_ID_ELEMENT:
        goto start_at_data_count_sec;
        break;
    case WP_WSA_BIN_MOD_SEC_ID_DATA_COUNT:
        goto start_at_code_sec;
        break;
    case WP_WSA_BIN_MOD_SEC_ID_CODE:
        goto start_at_data_sec;
        break;
    case WP_WSA_BIN_MOD_SEC_ID_DATA:
        goto start_at_custom_sec;
        break;
    default:
        return NULL;
    }

start_at_type_sec:
    const uint8_t *type_index;
    if (section_id == WP_WSA_BIN_MOD_SEC_ID_TYPE)
    {
        // Type Section
        index = ReadBinSection(index, &mod->typesec);
        if (!index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            // TODO set error id for invalid type section
            self->err.id = 1;
            self->err.module_id = 1;
            self->err.code = 0;
            return NULL;
        }

        type_index = mod->typesec.content;                 // Pointer to the start of the type section content
        type_index = DecodeLeb128UInt32(type_index, &len); // Get the number of types in the section
        if (!type_index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            // TODO set error id for invalid type section
            self->err.id = 2;
            self->err.module_id = 1;
            self->err.code = 0;
            return NULL; // No types in the section
        }
        mod->type_count = len; // Store the number of function types in the module

        for (i = 0; i < len; i++)
        {
            type_index = GetTypeByIndex(mod->typesec, i);
            if (!type_index)
            {
                mod->status = WP_MODULE_STATUS_ERROR;
                // TODO set error id for invalid type section
                self->err.id = 3;
                self->err.module_id = 1;
                self->err.code = 0;
                return NULL; // Invalid function type
            }
            err_code = ValidateTypeBuf(type_index, len);
            if (err_code > 0)
            {
                mod->status = WP_MODULE_STATUS_INVALID;
                // TODO set error id for invalid type section
                self->err.id = 4;
                self->err.module_id = 1;
                self->err.code = err_code;
                return NULL; // Invalid function type
            }
        }

        // Set the context types
        // self->c.functypes_count = mod->functype_count;
        // self->c.types = mod->typesec.content; // Pointer to the types section content
        // previous section id is now type section
        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_TYPE;
        return index;
    }

start_at_import_sec:
    const uint8_t *import_index;
    if (section_id == WP_WSA_BIN_MOD_SEC_ID_IMPORT)
    {
        // import Section
        index = ReadBinSection(index, &mod->importsec);
        if (!index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            self->err.id = 5; // TODO set error id for invalid import section
            self->err.module_id = 1;
            self->err.code = 0;
            return NULL;
        }
        import_index = mod->importsec.content;                 // Pointer to the start of the import section content
        import_index = DecodeLeb128UInt32(import_index, &len); // Get the number of imports in the section
        if (!import_index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            self->err.id = 6; // TODO set error id for invalid import section
            self->err.module_id = 1;
            self->err.code = 0;
            return NULL; // No imports in the section
        }
        mod->import_count = len; // Store the number of imports in the module

        for (i = 0; i < len; i++)
        {
            import_index = GetImportByIndex(mod->importsec, i);
            if (!import_index)
            {
                mod->status = WP_MODULE_STATUS_ERROR;
                self->err.id = 7; // TODO set error id for invalid import section
                self->err.module_id = 1;
                self->err.code = 0;
                return NULL; // Invalid import
            }
            // Validate import
            err_code = ValidateImportBuf(import_index, mod->import_count);
            if (err_code > 0)
            {
                mod->status = WP_MODULE_STATUS_INVALID;
                self->err.id = 8; // TODO set error id for invalid import section
                self->err.module_id = 1;
                self->err.code = err_code;
                return NULL; // Invalid import
            }
        }

        // previous section id is now import section
        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_IMPORT;
        return index;
    }

start_at_function_sec:
    const uint8_t *function_index;
    uint32_t function_type_index;
    if (section_id == WP_WSA_BIN_MOD_SEC_ID_FUNCTION)
    {
        // Function Section
        index = ReadBinSection(index, &mod->functionsec);
        if (!index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            self->err.id = 9; // TODO set error id for invalid function section
            self->err.module_id = 1;
            self->err.code = 0;
            return NULL;
        }

        function_index = mod->functionsec.content;                 // Pointer to the start of the function section content
        function_index = DecodeLeb128UInt32(function_index, &len); // Get the number of functions in the section
        if (!function_index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            self->err.id = 10; // TODO set error id for invalid function section
            self->err.module_id = 1;
            self->err.code = 0;
            return NULL; // No functions in the section
        }
        mod->function_count = len; // Store the number of functions in the module
        for (i = 0; i < len; i++)
        {
            function_index = GetFunctionByIndex(mod->functionsec, i);
            if (!function_index)
            {
                mod->status = WP_MODULE_STATUS_ERROR;
                self->err.id = 11; // TODO set error id for invalid function section
                self->err.module_id = 1;
                self->err.code = 0;
                return NULL; // Invalid function
            }
            // Validate function
            function_index = DecodeLeb128UInt32(function_index, &function_type_index); // Get the function type index
            if (!function_index)
            {
                mod->status = WP_MODULE_STATUS_ERROR;
                self->err.id = 12; // TODO set error id for invalid function section
                self->err.module_id = 1;
                self->err.code = 0;
                return NULL; // Invalid function type index
            }
            if (function_type_index >= mod->type_count)
            {
                mod->status = WP_MODULE_STATUS_INVALID;
                self->err.id = 13; // TODO set error id for invalid function section
                self->err.module_id = 1;
                self->err.code = 0;
                return NULL; // Invalid function type index
            }
        }
        // previous section id is now function section
        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_FUNCTION;
        return index;
    }

start_at_table_sec:
    const uint8_t *table_index;
    if (section_id == WP_WSA_BIN_MOD_SEC_ID_TABLE)
    {
        // Table Section
        index = ReadBinSection(index, &mod->tablesec);
        if (!index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            self->err.id = 14; // TODO set error id for invalid table section
            self->err.module_id = 1;
            self->err.code = 0;
            return NULL;
        }

        table_index = mod->tablesec.content;                 // Pointer to the start of the table section content
        table_index = DecodeLeb128UInt32(table_index, &len); // Get the number of tables in the section
        if (!table_index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            self->err.id = 15; // TODO set error id for invalid table section
            self->err.module_id = 1;
            self->err.code = 0;
            return NULL; // No tables in the section
        }
        mod->table_count = len; // Store the number of tables in the module

        /// The table type tabletype must be valid.
        for (i = 0; i < len; i++)
        {
            table_index = GetTableByIndex(mod->tablesec, i);
            if (!table_index)
            {
                mod->status = WP_MODULE_STATUS_ERROR;
                self->err.id = 16; // TODO set error id for invalid table section
                self->err.module_id = 1;
                self->err.code = 0;
                return NULL; // Invalid table
            }
            err_code = ValidateTableTypeBuf(table_index);
            if (err_code > 0)
            {
                mod->status = WP_MODULE_STATUS_INVALID;
                self->err.id = 17; // TODO set error id for invalid table section
                self->err.module_id = 1;
                self->err.code = err_code;
                // Invalid table limits
                return NULL;
            }
        }

        // previous section id is now table section
        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_TABLE;
        return index;
    }

start_at_memory_sec:
    const uint8_t *memory_index;
    if (section_id == WP_WSA_BIN_MOD_SEC_ID_MEMORY)
    {
        // Memory Section
        index = ReadBinSection(index, &mod->memsec);
        if (!index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            self->err.id = 18; // TODO set error id for invalid memory section
            self->err.module_id = 1;
            self->err.code = 0;
            return NULL;
        }

        memory_index = mod->memsec.content;                    // Pointer to the start of the memory section content
        memory_index = DecodeLeb128UInt32(memory_index, &len); // Get the number of memories in the section
        if (!memory_index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            self->err.id = 19; // TODO set error id for invalid memory section
            self->err.module_id = 1;
            self->err.code = 0;
            return NULL; // No memories in the section
        }
        mod->memory_count = len; // Store the number of memories in the module

        /// The memory type memtype must be valid.
        for (i = 0; i < len; i++)
        {
            memory_index = GetMemByIndex(mod->memsec, i);
            if (!memory_index)
            {
                mod->status = WP_MODULE_STATUS_ERROR;
                self->err.id = 20; // TODO set error id for invalid memory section
                self->err.module_id = 1;
                self->err.code = 0;
                return NULL; // Invalid memory
            }
            // Validate memory type
            err_code = ValidateMemTypeBuf(memory_index);
            if (err_code > 0)
            {
                mod->status = WP_MODULE_STATUS_INVALID;
                self->err.id = 21; // TODO set error id for invalid memory limits
                self->err.module_id = 1;
                self->err.code = 0;
                // Invalid memory limits
                return NULL;
            }
        }

        // previous section id is now memory section
        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_MEMORY;
        return index;
    }

start_at_tag_sec:
    const uint8_t *tag_index;
    if (section_id == WP_WSA_BIN_MOD_SEC_ID_TAG)
    {

        // Read tag section
        index = ReadBinSection(index, &mod->globalsec);
        if (!index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            self->err.id = 22; // TODO set error id for invalid tag section
            self->err.module_id = 1;
            self->err.code = 0;
            return NULL;
        }
        tag_index = mod->tagsec.content;                 // Pointer to the start of the tag section content
        tag_index = DecodeLeb128UInt32(tag_index, &len); // Get the number of tags in the section
        if (!tag_index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            self->err.id = 23; // TODO set error id for invalid tag section
            self->err.module_id = 1;
            self->err.code = 0;
            return NULL; // No globals in the section
        }
        mod->tag_count = len; // Store tag's number on the module state

        for (i = 0; i < len; i++)
        {
            /// The global type must be valid.
            tag_index = GetTagByIndex(mod->tagsec, i);
            if (!tag_index)
            {
                mod->status = WP_MODULE_STATUS_ERROR;
                self->err.id = 24; // TODO set error id for invalid tag section
                self->err.module_id = 1;
                self->err.code = 0;
                return NULL; // Invalid global
            }
            err_code = ValidateTagTypeBuf(tag_index);
            if (err_code > 0)
            {
                mod->status = WP_MODULE_STATUS_INVALID;
                self->err.id = 25; // TODO set error id for invalid tag type
                self->err.module_id = 1;
                self->err.code = err_code;
                return NULL; // Invalid global type
            }
        }

        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_TAG;
        return index;
    }

start_at_global_sec:
    const uint8_t *global_index;

    if (section_id == WP_WSA_BIN_MOD_SEC_ID_GLOBAL)
    {
        // GLOBAL Section
        index = ReadBinSection(index, &mod->globalsec);
        if (!index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            self->err.id = 26; // TODO set error id for invalid global section
            self->err.module_id = 1;
            self->err.code = 0;
            return NULL;
        }
        global_index = mod->globalsec.content;                 // Pointer to the start of the global section content
        global_index = DecodeLeb128UInt32(global_index, &len); // Get the number of globals in the section
        if (!global_index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            self->err.id = 27; // TODO set error id for invalid global type
            self->err.module_id = 1;
            self->err.code = 0;
            return NULL; // No globals in the section
        }
        mod->global_count = len; // Store the number of globals in the module

        for (i = 0; i < len; i++)
        {
            /// The global type must be valid.
            global_index = GetGlobalByIndex(mod->globalsec, i);
            if (!global_index)
            {
                mod->status = WP_MODULE_STATUS_ERROR;
                self->err.id = 28; // TODO set error id for invalid global type
                self->err.module_id = 1;
                self->err.code = 0;
                return NULL; // Invalid global
            }
            err_code = ValidateGlobalTypeBuf(global_index);
            if (err_code > 0)
            {
                mod->status = WP_MODULE_STATUS_INVALID;
                self->err.id = 29; // TODO set error id for invalid global type
                self->err.module_id = 1;
                self->err.code = err_code;
                return NULL; // Invalid global type
            }
            // Validate global init expr
            global_index += 2; // Move pointer to the start of the init expr
            err_code = ValidateConstantExprBuf(global_index, 0xFFFFFFFF);
            if (err_code > 0)
            {
                mod->status = WP_MODULE_STATUS_INVALID;
                self->err.id = 30; // TODO set error id for invalid global init expr
                self->err.module_id = 1;
                self->err.code = err_code;
                return NULL; // Invalid global init expr
            }
        }
        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_GLOBAL;
        return index;
    }

start_at_export_sec:
    const uint8_t *export_index;
    if (section_id == WP_WSA_BIN_MOD_SEC_ID_EXPORT)
    {
        // Export Section
        index = ReadBinSection(index, &mod->exportsec);
        if (!index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            self->err.id = 31; // TODO set error id for invalid export section
            self->err.module_id = 1;
            self->err.code = 0;
            return NULL;
        }
        export_index = mod->exportsec.content;                 // Pointer to the start of the export section content
        export_index = DecodeLeb128UInt32(export_index, &len); // Get the number of exports in the section
        if (!export_index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            self->err.id = 32; // TODO set error id for invalid export section
            self->err.module_id = 1;
            self->err.code = 0;
            return NULL; // No exports in the section
        }
        mod->export_count = len; // Store the number of exports in the module

        for (i = 0; i < len; i++)
        {
            export_index = GetExportByIndex(mod->exportsec, i);
            if (!export_index)
            {
                mod->status = WP_MODULE_STATUS_ERROR;
                self->err.id = 33; // TODO set error id for invalid export section
                self->err.module_id = 1;
                self->err.code = 0;
                return NULL; // Invalid export
            }
            // Validate export
            err_code = ValidateExportBuf(export_index, mod->function_count, mod->table_count, mod->memory_count, mod->global_count);
            if (err_code > 0)
            {
                mod->status = WP_MODULE_STATUS_INVALID;
                self->err.id = 34; // TODO set error id for invalid export
                self->err.module_id = 1;
                self->err.code = err_code;
                return NULL; // Invalid export
            }
        }
        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_EXPORT;
        return index;
    }

start_at_start_sec:
    const uint8_t *start_index;
    if (section_id == WP_WSA_BIN_MOD_SEC_ID_START)
    {
        // Start Section
        index = ReadBinSection(index, &mod->startsec);
        if (!index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            self->err.id = 35; // TODO set error id for invalid start section
            self->err.module_id = 1;
            self->err.code = 0;
            return NULL;
        }
        index = DecodeLeb128UInt32(index, &mod->start); // Get the start function index
        if (!index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            self->err.id = 36; // TODO set error id for invalid start function index
            self->err.module_id = 1;
            self->err.code = 0;
            return NULL;
        }
        // The start function index must refer to a function in the module.
        if (mod->start >= mod->function_count)
        {
            mod->status = WP_MODULE_STATUS_INVALID;
            self->err.id = 37; // TODO set error id for invalid start function index
            self->err.module_id = 1;
            self->err.code = 0;
            return NULL; // Invalid start function index
        }

        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_START;
        return index;
    }

start_at_element_sec:
    const uint8_t *element_index;
    if (section_id == WP_WSA_BIN_MOD_SEC_ID_ELEMENT)
    {
        // Element Section
        index = ReadBinSection(index, &mod->elemsec);
        if (!index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            self->err.id = 38; // TODO set error id for invalid element section
            self->err.module_id = 1;
            self->err.code = 0;
            return NULL;
        }
        element_index = mod->elemsec.content;                    // Pointer to the start of the element section content
        element_index = DecodeLeb128UInt32(element_index, &len); // Get the number of elements in the section
        if (!element_index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            self->err.id = 39; // TODO set error id for invalid element section
            self->err.module_id = 1;
            self->err.code = 0;
            return NULL; // No elements in the section
        }
        mod->element_count = len; // Store the number of elements in the module
        for (i = 0; i < len; i++)
        {
            element_index = GetElementByIndex(mod->elemsec, i);
            if (!element_index)
            {
                mod->status = WP_MODULE_STATUS_ERROR;
                self->err.id = 40; // TODO set error id for invalid element section
                self->err.module_id = 1;
                self->err.code = 0;
                return NULL; // Invalid element
            }
            // Validate element
            err_code = ValidateElementBuf(element_index, mod->function_count, mod->table_count);
            if (err_code > 0)
            {
                mod->status = WP_MODULE_STATUS_INVALID;
                self->err.id = 41; // TODO set error id for invalid element section
                self->err.module_id = 1;
                self->err.code = err_code;
                return NULL; // Invalid element
            }
        }

        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_ELEMENT;
        return index;
    }

start_at_data_count_sec:
    const uint8_t *data_count_index;
    if (section_id == WP_WSA_BIN_MOD_SEC_ID_DATA_COUNT)
    {
        // Data Count Section
        index = ReadBinSection(index, &mod->datacountsec);
        if (!index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            self->err.id = 42; // TODO set error id for invalid data count section
            self->err.module_id = 1;
            self->err.code = 0;
            return NULL;
        }
        data_count_index = mod->datacountsec.content;                              // Pointer to the start of the data count section content
        data_count_index = DecodeLeb128UInt32(data_count_index, &mod->data_count); // Get the number of data segments in the section
        if (!data_count_index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            self->err.id = 43; // TODO set error id for invalid data count section
            self->err.module_id = 1;
            self->err.code = 0;
            return NULL; // No data count in the section
        }

        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_DATA_COUNT;
        return index;
    }

start_at_code_sec:
    const uint8_t *code_index;
    if (section_id == WP_WSA_BIN_MOD_SEC_ID_CODE)
    {
        // Code Section
        index = ReadBinSection(index, &mod->codesec);
        if (!index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            self->err.id = 44; // TODO set error id for invalid code section
            self->err.module_id = 1;
            self->err.code = 0;
            return NULL;
        }
        code_index = mod->codesec.content;                 // Pointer to the start of the code section content
        code_index = DecodeLeb128UInt32(code_index, &len); // Get the number of code in the section
        if (!code_index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            self->err.id = 45; // TODO set error id for invalid code section
            self->err.module_id = 1;
            self->err.code = 0;
            return NULL; // No code in the section
        }
        if (len != mod->function_count)
        {
            mod->status = WP_MODULE_STATUS_INVALID;
            self->err.id = 46; // TODO set error id for mismatched function and code counts
            self->err.module_id = 1;
            self->err.code = 0;
            return NULL; // The number of code entries must match the number of functions
        }

        for (i = 0; i < len; i++)
        {
            code_index = GetCodeByIndex(mod->codesec, i);
            if (!code_index)
            {
                mod->status = WP_MODULE_STATUS_ERROR;
                self->err.id = 47; // TODO set error id for invalid code section
                self->err.module_id = 1;
                self->err.code = 0;
                return NULL; // Invalid code
            }
            // Validate function
            err_code = ValidateCodeBuf(code_index);
            if (err_code > 0)
            {
                mod->status = WP_MODULE_STATUS_INVALID;
                self->err.id = 48; // TODO set error id for invalid code section
                self->err.module_id = 1;
                self->err.code = err_code;
                return NULL; // Invalid function
            }
        }

        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_CODE;
        return index;
    }

start_at_data_sec:
    const uint8_t *data_index;
    if (section_id == WP_WSA_BIN_MOD_SEC_ID_DATA)
    {
        // Data Section
        index = ReadBinSection(index, &mod->datasec);
        if (!index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            self->err.id = 49; // TODO set error id for invalid data section
            self->err.module_id = 1;
            self->err.code = 0;
            return NULL;
        }
        data_index = mod->datasec.content;                 // Pointer to the start of the data section content
        data_index = DecodeLeb128UInt32(data_index, &len); // Get the number of data segments in the section
        if (!data_index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            self->err.id = 50; // TODO set error id for invalid data section
            self->err.module_id = 1;
            self->err.code = 0;
            return NULL; // No data segments in the section
        }
        if (len != mod->data_count)
        {
            mod->status = WP_MODULE_STATUS_INVALID;
            self->err.id = 51; // TODO set error id for mismatched data segment count
            self->err.module_id = 1;
            self->err.code = 0;
            return NULL; // The number of data segments must match the data count
        }

        for (i = 0; i < len; i++)
        {
            data_index = GetDataByIndex(mod->datasec, i);
            if (!data_index)
            {
                mod->status = WP_MODULE_STATUS_ERROR;
                self->err.id = 52; // TODO set error id for invalid data segment
                self->err.module_id = 1;
                self->err.code = 0;
                return NULL; // Invalid data segment
            }
            // Validate data segment
            err_code = ValidateDataBuf(data_index, mod->memory_count);
            if (err_code > 0)
            {
                mod->status = WP_MODULE_STATUS_INVALID;
                self->err.id = 53; // TODO set error id for invalid data segment
                self->err.module_id = 1;
                self->err.code = err_code;
                return NULL; // Invalid data segment
            }
        }

        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_DATA;
        return index;
    }

start_at_custom_sec:
    if (section_id == WP_WSA_BIN_MOD_SEC_ID_CUSTOM)
    {
        // custom section will be ignored
        //  Has no use on module execution.
        index = DecodeLeb128UInt32(index, &aux_u32);
        if (!index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            self->err.id = 54; // TODO set error id for invalid custom section
            self->err.module_id = 1;
            self->err.code = 0;
            return NULL;
        }
        index = index + aux_u32;
        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_CUSTOM;
        return index;
    }

    return NULL;
}
