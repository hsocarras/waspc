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

#include "validator/wasm_validator.h"
#include "validator/wasm_validator_private.h"
#include "decoder/wasm_decoder.h"
#include "webassembly/instructions.h"
#include "decoder/leb128.h"

#include <stdint.h>

///Static function ///////////////////////////////////////////////////////////////////////////////////////////////////
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
 * @brief Function to decode and validate each section inside a module.
 * Checks if the section is in the correct order and validates binary sintax on each section.
 * @param self Pointer to the validator state
 * @param index
 * @param section_id
 * @param previous_secction
 * @param mod
 * @return const uint8_t* pointer to next section or NULL if error
 */
static const uint8_t *ValidateBinSectionById(WpValidatorState *self, const uint8_t *index, const uint8_t section_id, uint8_t *previous_secction)
{
    const uint8_t * section_index;
    const uint8_t * section_end;
    uint32_t aux_u32; // auxiliary var to store u32 values
    uint32_t len;
    uint32_t i;
    uint32_t err_code;
    WpModuleState *mod = self->mod;

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
    if (section_id == WP_WSA_BIN_MOD_SEC_ID_TYPE)
    {
        // Type Section
        index = ReadBinSection(index, &mod->typesec);
        if (!index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            // TODO set error id for invalid type section
            self->err->id = 1;
            #ifdef WASPC_CONFIG_DEV_FLAG
            self->err->module_id = 1;
            self->err->func_code = 2;
            self->err->block_code = 1;
            #endif
            return NULL;
        }

        section_index = mod->typesec.content;                 // Pointer to the start of the type section content
        section_index = DecodeLeb128UInt32(section_index, &len); // Get the number of types in the section
        if (!section_index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            // TODO set error id for invalid type section
            self->err->id = 2;
            #ifdef WASPC_CONFIG_DEV_FLAG
            self->err->module_id = 1;
            self->err->func_code = 2;
            self->err->block_code = 2;
            #endif
            return NULL; // No types in the section
        }
        self->c.types_count = len; // Store the number of function types in the context

        for (i = 0; i < len; i++)
        {
            section_index = GetTypeByIndex(mod->typesec, i);
            if (!section_index)
            {
                mod->status = WP_MODULE_STATUS_ERROR;
                // TODO set error id for invalid type section
                self->err->id = 3;
                #ifdef WASPC_CONFIG_DEV_FLAG
                self->err->module_id = 1;
                self->err->func_code = 2;
                self->err->block_code = 3;
                #endif
                return NULL; // Invalid function type
            }
            err_code = ValidateTypeBuf(section_index, len);
            if (err_code > 0)
            {
                mod->status = WP_MODULE_STATUS_INVALID;
                // TODO set error id for invalid type section
                self->err->id = 4;
                #ifdef WASPC_CONFIG_DEV_FLAG
                self->err->module_id = 1;
                self->err->func_code = 2;
                self->err->block_code = 4;
                self->err->err_code = err_code;
                #endif
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
    if (section_id == WP_WSA_BIN_MOD_SEC_ID_IMPORT)
    {
        // import Section
        
        index = ReadBinSection(index, &mod->importsec);
        if (!index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            self->err->id = 5; // TODO set error id for invalid import section
            #ifdef WASPC_CONFIG_DEV_FLAG
            self->err->module_id = 1;
            self->err->func_code = 2;
            self->err->block_code = 5;
            #endif
            return NULL;
        }

        section_end = mod->importsec.content + mod->importsec.size;
        section_index = mod->importsec.content;                 // Pointer to the start of the import section content
        section_index = DecodeLeb128UInt32(section_index, &len); // Get the number of imports in the section
        if (!section_index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            self->err->id = 6; // TODO set error id for invalid import section
            #ifdef WASPC_CONFIG_DEV_FLAG
            self->err->module_id = 1;
            self->err->func_code = 2;
            self->err->block_code = 6;
            #endif
            return NULL; // No imports in the section
        }
        self->c.imports_count = len; // Store the number of imports in the module

        for (i = 0; i < len; i++)
        {
            section_index = GetImportByIndex(mod->importsec, i);
            if (!section_index)
            {
                mod->status = WP_MODULE_STATUS_ERROR;
                self->err->id = 7; // TODO set error id for invalid import section
                #ifdef WASPC_CONFIG_DEV_FLAG
                self->err->module_id = 1;
                self->err->func_code = 2;
                self->err->block_code = 7;
                #endif
                return NULL; // Invalid import
            }
            // Validate import
            err_code = ValidateImport(self, section_index, section_end);
            if (err_code > 0)
            {
                mod->status = WP_MODULE_STATUS_INVALID;
                self->err->id = 8; // TODO set error id for invalid import section
                #ifdef WASPC_CONFIG_DEV_FLAG
                self->err->module_id = 1;
                self->err->func_code = 2;
                self->err->block_code = 8;
                self->err->err_code = err_code;
                #endif
                return NULL; // Invalid import
            }
        }

        // previous section id is now import section
        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_IMPORT;
        return index;
    }

start_at_function_sec:
    if (section_id == WP_WSA_BIN_MOD_SEC_ID_FUNCTION)
    {
        // Function Section
        index = ReadBinSection(index, &mod->functionsec);
        if (!index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            self->err->id = 9; // TODO set error id for invalid function section
            #ifdef WASPC_CONFIG_DEV_FLAG
            self->err->module_id = 1;
            self->err->func_code = 2;
            self->err->block_code = 9;
            #endif
            return NULL;
        }

        section_index = mod->functionsec.content;                 // Pointer to the start of the function section content
        section_index = DecodeLeb128UInt32(section_index, &len); // Get the number of functions in the section
        if (!section_index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            self->err->id = 10; // TODO set error id for invalid function section
            #ifdef WASPC_CONFIG_DEV_FLAG
            self->err->module_id = 1;
            self->err->func_code = 2;
            self->err->block_code = 10;
            #endif
            return NULL; // No functions in the section
        }
        self->c.functions_count =  len; // Store the number of functions in the context
        for (i = 0; i < len; i++)
        {
            section_index = GetFunctionByIndex(mod->functionsec, i);
            if (!section_index)
            {
                mod->status = WP_MODULE_STATUS_ERROR;
                self->err->id = 11; // TODO set error id for invalid function section
                #ifdef WASPC_CONFIG_DEV_FLAG
                self->err->module_id = 1;
                self->err->func_code = 2;
                self->err->block_code = 11;
                #endif
                return NULL; // Invalid function
            }
            // Validate function
            section_index = DecodeLeb128UInt32(section_index, &aux_u32); // Get the function type index
            if (!section_index)
            {
                mod->status = WP_MODULE_STATUS_ERROR;
                self->err->id = 12; // TODO set error id for invalid function section
                #ifdef WASPC_CONFIG_DEV_FLAG
                self->err->module_id = 1;
                self->err->func_code = 2;
                self->err->block_code = 12;
                #endif
                return NULL; // Invalid function type index
            }
            if (aux_u32 >= self->c.types_count)
            {
                mod->status = WP_MODULE_STATUS_INVALID;
                self->err->id = 13; // TODO set error id for invalid function section
                #ifdef WASPC_CONFIG_DEV_FLAG
                self->err->module_id = 1;
                self->err->func_code = 2;
                self->err->block_code = 13;
                #endif
                return NULL; // Invalid function type index
            }
        }
        // previous section id is now function section
        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_FUNCTION;
        return index;
    }

start_at_table_sec:
    if (section_id == WP_WSA_BIN_MOD_SEC_ID_TABLE)
    {
        // Table Section
        index = ReadBinSection(index, &mod->tablesec);
        if (!index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            self->err->id = 14; // TODO set error id for invalid table section
            #ifdef WASPC_CONFIG_DEV_FLAG
            self->err->module_id = 1;
            self->err->func_code = 2;
            self->err->block_code = 14;
            #endif
            return NULL;
        }

        section_index = mod->tablesec.content;                 // Pointer to the start of the table section content
        section_index = DecodeLeb128UInt32(section_index, &len); // Get the number of tables in the section
        if (!section_index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            self->err->id = 15; // TODO set error id for invalid table section
            #ifdef WASPC_CONFIG_DEV_FLAG
            self->err->module_id = 1;
            self->err->func_code = 2;
            self->err->block_code = 15;
            #endif
            return NULL; // No tables in the section
        }
        self->c.tables_count = len; // Store the number of tables in the module

        /// The table type tabletype must be valid.
        for (i = 0; i < len; i++)
        {
            section_index = GetTableByIndex(mod->tablesec, i);
            if (!section_index)
            {
                mod->status = WP_MODULE_STATUS_ERROR;
                self->err->id = 16; // TODO set error id for invalid table section
                #ifdef WASPC_CONFIG_DEV_FLAG
                self->err->module_id = 1;
                self->err->func_code = 2;
                self->err->block_code = 16;
                #endif
                return NULL; // Invalid table
            }
            err_code = ValidateTableTypeBuf(section_index);
            if (err_code > 0)
            {
                mod->status = WP_MODULE_STATUS_INVALID;
                self->err->id = 17; // TODO set error id for invalid table section
                #ifdef WASPC_CONFIG_DEV_FLAG
                self->err->module_id = 1;
                self->err->func_code = 2;
                self->err->block_code = 17;
                #endif
                // Invalid table limits
                return NULL;
            }
        }

        // previous section id is now table section
        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_TABLE;
        return index;
    }

start_at_memory_sec:
    if (section_id == WP_WSA_BIN_MOD_SEC_ID_MEMORY)
    {
        // Memory Section
        index = ReadBinSection(index, &mod->memsec);

        if (!index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            self->err->id = 18; // TODO set error id for invalid memory section
           #ifdef WASPC_CONFIG_DEV_FLAG
            self->err->module_id = 1;
            self->err->func_code = 2;
            self->err->block_code = 18;
            #endif
            return NULL;
        }

        section_index = mod->memsec.content;                    // Pointer to the start of the memory section content
        section_index = DecodeLeb128UInt32(section_index, &len); // Get the number of memories in the section
        if (!section_index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            self->err->id = 19; // TODO set error id for invalid memory section
            #ifdef WASPC_CONFIG_DEV_FLAG
            self->err->module_id = 1;
            self->err->func_code = 2;
            self->err->block_code = 19;
            #endif
            return NULL; // No memories in the section
        }
        self->c.mems_count = len; // Store the number of memories in the module

        /// The memory type memtype must be valid.
        for (i = 0; i < len; i++)
        {
            section_index = GetMemByIndex(mod->memsec, i);
            if (!section_index)
            {
                mod->status = WP_MODULE_STATUS_ERROR;
                self->err->id = 20; // TODO set error id for invalid memory section
                #ifdef WASPC_CONFIG_DEV_FLAG
                self->err->module_id = 1;
                self->err->func_code = 2;
                self->err->block_code = 20;
                #endif
                return NULL; // Invalid memory
            }
            // Validate memory type
            err_code = ValidateMemTypeBuf(section_index);
            if (err_code > 0)
            {
                mod->status = WP_MODULE_STATUS_INVALID;
                self->err->id = 21; // TODO set error id for invalid memory limits
                #ifdef WASPC_CONFIG_DEV_FLAG
                self->err->module_id = 1;
                self->err->func_code = 2;
                self->err->block_code = 21;
                self->err->err_code = err_code;
                #endif
                // Invalid memory limits
                return NULL;
            }
        }

        // previous section id is now memory section
        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_MEMORY;
        return index;
    }

start_at_tag_sec:
    if (section_id == WP_WSA_BIN_MOD_SEC_ID_TAG)
    {

        // Read tag section
        index = ReadBinSection(index, &mod->globalsec);
        if (!index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            self->err->id = 22; // TODO set error id for invalid tag section
            #ifdef WASPC_CONFIG_DEV_FLAG
            self->err->module_id = 1;
            self->err->func_code = 2;
            self->err->block_code = 22;
            #endif
            return NULL;
        }
        section_index = mod->tagsec.content;                 // Pointer to the start of the tag section content
        section_index = DecodeLeb128UInt32(section_index, &len); // Get the number of tags in the section
        if (!section_index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            self->err->id = 23; // TODO set error id for invalid tag section
            #ifdef WASPC_CONFIG_DEV_FLAG
            self->err->module_id = 1;
            self->err->func_code = 2;
            self->err->block_code = 23;
            #endif
            return NULL; // No globals in the section
        }
        self->c.tags_count = len; // Store tag's number on the module state

        for (i = 0; i < len; i++)
        {
            /// The global type must be valid.
            section_index = GetTagByIndex(mod->tagsec, i);
            if (!section_index)
            {
                mod->status = WP_MODULE_STATUS_ERROR;
                self->err->id = 24; // TODO set error id for invalid tag section
                #ifdef WASPC_CONFIG_DEV_FLAG
                self->err->module_id = 1;
                self->err->func_code = 2;
                self->err->block_code = 24;
                #endif
                return NULL; // Invalid global
            }
            err_code = ValidateTagTypeBuf(section_index);
            if (err_code > 0)
            {
                mod->status = WP_MODULE_STATUS_INVALID;
                self->err->id = 25; // TODO set error id for invalid tag type
                #ifdef WASPC_CONFIG_DEV_FLAG
                self->err->module_id = 1;
                self->err->func_code = 2;
                self->err->block_code = 25;
                self->err->err_code = err_code;
                #endif
                return NULL; // Invalid global type
            }
        }

        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_TAG;
        return index;
    }

start_at_global_sec:
    if (section_id == WP_WSA_BIN_MOD_SEC_ID_GLOBAL)
    {
        // GLOBAL Section
        index = ReadBinSection(index, &mod->globalsec);
        if (!index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            self->err->id = 26; // TODO set error id for invalid global section
            #ifdef WASPC_CONFIG_DEV_FLAG
            self->err->module_id = 1;
            self->err->func_code = 2;
            self->err->block_code = 26;
            #endif
            return NULL;
        }

        section_end = mod->globalsec.content + mod->globalsec.size;
        section_index = mod->globalsec.content;                 // Pointer to the start of the global section content
        section_index = DecodeLeb128UInt32(section_index, &len); // Get the number of globals in the section
        if (!section_index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            self->err->id = 27; // TODO set error id for invalid global type
            #ifdef WASPC_CONFIG_DEV_FLAG
            self->err->module_id = 1;
            self->err->func_code = 2;
            self->err->block_code = 27;
            #endif
            return NULL; // No globals in the section
        }
        self->c.globals_count = len; // Store the number of globals in the module
        for (i = 0; i < len; i++)
        {
            /// The global type must be valid.
            section_index = GetGlobalByIndex(mod->globalsec, i);
            if (!section_index)
            {
                mod->status = WP_MODULE_STATUS_ERROR;
                self->err->id = 28; // TODO set error id for invalid global type
                #ifdef WASPC_CONFIG_DEV_FLAG
                self->err->module_id = 1;
                self->err->func_code = 2;
                self->err->block_code = 28;
                #endif
                return NULL; // Invalid global
            }
            err_code = ValidateGlobal(self, section_index, section_end);
            if (err_code > 0)
            {
                mod->status = WP_MODULE_STATUS_INVALID;
                self->err->id = 29; // TODO set error id for invalid global type
                #ifdef WASPC_CONFIG_DEV_FLAG
                self->err->module_id = 1;
                self->err->func_code = 2;
                self->err->block_code = 29;
                self->err->err_code = err_code;
                #endif
                return NULL; // Invalid global type
            }
        }
        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_GLOBAL;
        return index;
    }

start_at_export_sec:    
    if (section_id == WP_WSA_BIN_MOD_SEC_ID_EXPORT)
    {
        // Export Section
        index = ReadBinSection(index, &mod->exportsec);
        if (!index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            self->err->id = 31; // TODO set error id for invalid export section
            #ifdef WASPC_CONFIG_DEV_FLAG
                self->err->module_id = 1;
                self->err->func_code = 2;
                self->err->block_code = 30;
                self->err->err_code = 0;
            #endif
            return NULL;
        }
        section_index = mod->exportsec.content;                 // Pointer to the start of the export section content
        section_end = section_index + mod->exportsec.size;
        section_index = DecodeLeb128UInt32(section_index, &len); // Get the number of exports in the section
        if (!section_index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            self->err->id = 32; // TODO set error id for invalid export section
            #ifdef WASPC_CONFIG_DEV_FLAG
                self->err->module_id = 1;
                self->err->func_code = 2;
                self->err->block_code = 31;
                self->err->err_code = 0;
            #endif
            return NULL; // No exports in the section
        }
        self->c.exports_count = len; // Store the number of exports in the module
        for (i = 0; i < len; i++)
        {
            section_index = GetExportByIndex(mod->exportsec, i);
            if (!section_index)
            {
                mod->status = WP_MODULE_STATUS_ERROR;
                self->err->id = 33; // TODO set error id for invalid export section
                #ifdef WASPC_CONFIG_DEV_FLAG
                    self->err->module_id = 1;
                    self->err->func_code = 2;
                    self->err->block_code = 32;
                    self->err->err_code = 0;
                #endif
                return NULL; // Invalid export
            }
            // Validate export
            err_code = ValidateExport(self, section_index, section_end);
            if (err_code > 0)
            {   
                mod->status = WP_MODULE_STATUS_INVALID;
                self->err->id = 34; // TODO set error id for invalid export
                #ifdef WASPC_CONFIG_DEV_FLAG
                    self->err->module_id = 1;
                    self->err->func_code = 2;
                    self->err->block_code = 33;
                    self->err->err_code = err_code;
                #endif
                return NULL; // Invalid export
            }
        }
        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_EXPORT;
        return index;
    }

start_at_start_sec:
    if (section_id == WP_WSA_BIN_MOD_SEC_ID_START)
    {
        // Start Section
        index = ReadBinSection(index, &mod->startsec);
        if (!index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            self->err->id = 35; // TODO set error id for invalid start section
            #ifdef WASPC_CONFIG_DEV_FLAG
                self->err->module_id = 1;
                self->err->func_code = 2;
                self->err->block_code = 34;
                self->err->err_code = 0;
            #endif
            return NULL;
        }
        index = DecodeLeb128UInt32(index, &aux_u32); // Get the start function index
        if (!index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            self->err->id = 36; // TODO set error id for invalid start function index
            #ifdef WASPC_CONFIG_DEV_FLAG
                self->err->module_id = 1;
                self->err->func_code = 2;
                self->err->block_code = 35;
                self->err->err_code = 0;
            #endif
            return NULL;
        }
        // The start function index must refer to a function in the module.
        if (aux_u32 >= self->c.functions_count)
        {
            mod->status = WP_MODULE_STATUS_INVALID;
            self->err->id = 37; // TODO set error id for invalid start function index
            #ifdef WASPC_CONFIG_DEV_FLAG
                self->err->module_id = 1;
                self->err->func_code = 2;
                self->err->block_code = 36;
                self->err->err_code = 0;
            #endif
            return NULL; // Invalid start function index
        }

        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_START;
        return index;
    }

start_at_element_sec:
    if (section_id == WP_WSA_BIN_MOD_SEC_ID_ELEMENT)
    {
        // Element Section
        index = ReadBinSection(index, &mod->elemsec);
        if (!index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            self->err->id = 38; // TODO set error id for invalid element section
            #ifdef WASPC_CONFIG_DEV_FLAG
                self->err->module_id = 1;
                self->err->func_code = 2;
                self->err->block_code = 37;
                self->err->err_code = 0;
            #endif
            return NULL;
        }
        section_index = mod->elemsec.content;                    // Pointer to the start of the element section content
        section_index = DecodeLeb128UInt32(section_index, &len); // Get the number of elements in the section
        if (!section_index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            self->err->id = 39; // TODO set error id for invalid element section
            #ifdef WASPC_CONFIG_DEV_FLAG
                self->err->module_id = 1;
                self->err->func_code = 2;
                self->err->block_code = 38;
                self->err->err_code = 0;
            #endif
            return NULL; // No elements in the section
        }
        self->c.elements_count = len; // Store the number of elements in the module
        for (i = 0; i < len; i++)
        {
            section_index = GetElementByIndex(mod->elemsec, i);
            if (!section_index)
            {
                mod->status = WP_MODULE_STATUS_ERROR;
                self->err->id = 40; // TODO set error id for invalid element section
                #ifdef WASPC_CONFIG_DEV_FLAG
                    self->err->module_id = 1;
                    self->err->func_code = 2;
                    self->err->block_code = 39;
                    self->err->err_code = 0;
                #endif
                return NULL; // Invalid element
            }
            // Validate element
            err_code = ValidateElementBuf(section_index, self->c.functions_count, self->c.tables_count);
            if (err_code > 0)
            {
                mod->status = WP_MODULE_STATUS_INVALID;
                self->err->id = 41; // TODO set error id for invalid element section
                #ifdef WASPC_CONFIG_DEV_FLAG
                    self->err->module_id = 1;
                    self->err->func_code = 2;
                    self->err->block_code = 40;
                    self->err->err_code = err_code;
                #endif
                return NULL; // Invalid element
            }
        }

        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_ELEMENT;
        return index;
    }

start_at_data_count_sec:
    if (section_id == WP_WSA_BIN_MOD_SEC_ID_DATA_COUNT)
    {
        // Data Count Section
        index = ReadBinSection(index, &mod->datacountsec);
        if (!index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            self->err->id = 42; // TODO set error id for invalid data count section
            #ifdef WASPC_CONFIG_DEV_FLAG
                self->err->module_id = 1;
                self->err->func_code = 2;
                self->err->block_code = 41;
                self->err->err_code = 0;
            #endif
            return NULL;
        }
        section_index = mod->datacountsec.content;                              // Pointer to the start of the data count section content
        section_index = DecodeLeb128UInt32(section_index, &self->c.data_count); // Get the number of data segments in the section
        if (!section_index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            self->err->id = 43; // TODO set error id for invalid data count section
            #ifdef WASPC_CONFIG_DEV_FLAG
                self->err->module_id = 1;
                self->err->func_code = 2;
                self->err->block_code = 42;
                self->err->err_code = 0;
            #endif
            return NULL; // No data count in the section
        }

        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_DATA_COUNT;
        return index;
    }

start_at_code_sec:
    if (section_id == WP_WSA_BIN_MOD_SEC_ID_CODE)
    {
        // Code Section
        index = ReadBinSection(index, &mod->codesec);
        if (!index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            self->err->id = 44; // TODO set error id for invalid code section
            #ifdef WASPC_CONFIG_DEV_FLAG
                self->err->module_id = 1;
                self->err->func_code = 2;
                self->err->block_code = 43;
                self->err->err_code = 0;
            #endif
            return NULL;
        }
        section_index = mod->codesec.content;                 // Pointer to the start of the code section content
        section_index = DecodeLeb128UInt32(section_index, &len); // Get the number of code in the section
        if (!section_index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            self->err->id = 45; // TODO set error id for invalid code section
            #ifdef WASPC_CONFIG_DEV_FLAG
                self->err->module_id = 1;
                self->err->func_code = 2;
                self->err->block_code = 44;
                self->err->err_code = 0;
            #endif
            return NULL; // No code in the section
        }
        if (len != self->c.functions_count)
        {
            mod->status = WP_MODULE_STATUS_INVALID;
            self->err->id = 46; // TODO set error id for mismatched function and code counts
            #ifdef WASPC_CONFIG_DEV_FLAG
                self->err->module_id = 1;
                self->err->func_code = 2;
                self->err->block_code = 45;
                self->err->err_code = 0;
            #endif
            return NULL; // The number of code entries must match the number of functions
        }

        for (i = 0; i < len; i++)
        {
            section_index = GetCodeByIndex(mod->codesec, i);
            if (!section_index)
            {
                mod->status = WP_MODULE_STATUS_ERROR;
                self->err->id = 47; // TODO set error id for invalid code section
                #ifdef WASPC_CONFIG_DEV_FLAG
                    self->err->module_id = 1;
                    self->err->func_code = 2;
                    self->err->block_code = 46;
                    self->err->err_code = 0;
                #endif
                return NULL; // Invalid code
            }
            // Validate function
            err_code = ValidateCodeBuf(section_index);
            if (err_code > 0)
            {
                mod->status = WP_MODULE_STATUS_INVALID;
                self->err->id = 48; // TODO set error id for invalid code section
                #ifdef WASPC_CONFIG_DEV_FLAG
                    self->err->module_id = 1;
                    self->err->func_code = 2;
                    self->err->block_code = 47;
                    self->err->err_code = err_code;
                #endif
                return NULL; // Invalid function
            }
        }

        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_CODE;
        return index;
    }

start_at_data_sec:
    if (section_id == WP_WSA_BIN_MOD_SEC_ID_DATA)
    {
        // Data Section
        index = ReadBinSection(index, &mod->datasec);
        if (!index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            self->err->id = 49; // TODO set error id for invalid data section
            #ifdef WASPC_CONFIG_DEV_FLAG
                self->err->module_id = 1;
                self->err->func_code = 2;
                self->err->block_code = 48;
                self->err->err_code = 0;
            #endif
            return NULL;
        }
        section_index = mod->datasec.content;                 // Pointer to the start of the data section content
        section_index = DecodeLeb128UInt32(section_index, &len); // Get the number of data segments in the section
        if (!section_index)
        {
            mod->status = WP_MODULE_STATUS_ERROR;
            self->err->id = 50; // TODO set error id for invalid data section
            #ifdef WASPC_CONFIG_DEV_FLAG
                self->err->module_id = 1;
                self->err->func_code = 2;
                self->err->block_code = 49;
                self->err->err_code = 0;
            #endif
            return NULL; // No data segments in the section
        }
        if (len != self->c.data_count)
        {
            mod->status = WP_MODULE_STATUS_INVALID;
            self->err->id = 51; // TODO set error id for mismatched data segment count
            #ifdef WASPC_CONFIG_DEV_FLAG
                self->err->module_id = 1;
                self->err->func_code = 2;
                self->err->block_code = 50;
                self->err->err_code = 0;
            #endif
            return NULL; // The number of data segments must match the data count
        }

        for (i = 0; i < len; i++)
        {
            section_index = GetDataByIndex(mod->datasec, i);
            if (!section_index)
            {
                mod->status = WP_MODULE_STATUS_ERROR;
                self->err->id = 52; // TODO set error id for invalid data segment
                #ifdef WASPC_CONFIG_DEV_FLAG
                    self->err->module_id = 1;
                    self->err->func_code = 2;
                    self->err->block_code = 51;
                    self->err->err_code = 0;
                #endif
                return NULL; // Invalid data segment
            }
            // Validate data segment
            err_code = ValidateDataBuf(section_index, self->c.mems_count);
            if (err_code > 0)
            {
                mod->status = WP_MODULE_STATUS_INVALID;
                self->err->id = 53; // TODO set error id for invalid data segment
                #ifdef WASPC_CONFIG_DEV_FLAG
                    self->err->module_id = 1;
                    self->err->func_code = 2;
                    self->err->block_code = 52;
                    self->err->err_code = err_code;
                #endif
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
            self->err->id = 54; // TODO set error id for invalid custom section
            #ifdef WASPC_CONFIG_DEV_FLAG
                self->err->module_id = 1;
                self->err->func_code = 2;
                self->err->block_code = 53;
                self->err->err_code = 0;
            #endif
            return NULL;
        }
        index = index + aux_u32;
        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_CUSTOM;
        return index;
    }

    return NULL;
}


///////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////////

/**
 * @brief Function to initialize the validator state.
 */
void WpValidatorStateInit(WpValidatorState *self)
{
    self->err = NULL;           // Init error object
    self->mod = NULL;

    // Initialize the validator state context c
    self->c.types_count = 0;
    self->c.imports_count = 0;
    self->c.imports_functions_count = 0;
    self->c.imports_tables_count = 0;
    self->c.imports_mems_count = 0;
    self->c.imports_globals_count = 0;
    self->c.imports_tags_count = 0;
    self->c.functions_count = 0;
    self->c.tables_count = 0;
    self->c.mems_count = 0;
    self->c.globals_count = 0;
    self->c.tags_count = 0;
    self->c.exports_count = 0;
    self->c.elements_count = 0;
    self->c.data_count = 0;
    self->c.locals_count = 0;
    self->c.labels_count = 0;

    // Initialize the prime context
    // self->c_prime.types.lenght = 0;
    // self->c_prime.types.elements = NULL;
    // self->c_prime.locals.lenght = 0;
    // self->c_prime.locals.elements = NULL;
    // self->c_prime.return_types.lenght = 0;
    // self->c_prime.return_types.elements = NULL;


    // self->ctrl_stack = NULL;
    // self->ctr_stack_idx = 0;  // Initialize control stack index
    // self->ctr_stack_size = 0; // Set default control stack size

    self->value_stack = NULL;                    /// stack to store value types
    self->value_stack_top = NULL;                /// pointer to the top of the value stack
    self->value_stack_end = NULL;

    self->ip = NULL; // Initialize instruction pointer to NULL
}

uint32_t ValidateValType(const uint8_t *valtype)
{
    if (IsNumericType(valtype))
    {
        return 0; // Valid numeric type
    }    
    if (*valtype == 0x7B)
    {
        return 0; // Valid vector type
    }
    if (IsRefType(valtype))
    {
        return 0; // TODO Valid reference type
    }
    return 1; // Invalid value type
}

uint32_t ValidateImport(WpValidatorState *self, const uint8_t *index, const uint8_t *import_section_end)
{
    WasmBinImport import;
    const uint8_t *idx;
    uint32_t aux_u32;


    uint32_t err_code = ValidateImportBuf(index); // check for error on binary encoding
    if (err_code > 0)
    {
        return err_code;
    }

    // destructuring import
    import = DestructureImport(index);

    switch (import.external_type)
    {
    case 0x00:          //func idx
        idx = DecodeLeb128UInt32(import.external, &aux_u32);
        if (!idx)
        {
            return 1; //invalid function index
        }
        if (aux_u32 > self->c.types_count)
        {
            return 2; //invalid function type index
        }
        idx = GetTypeByIndex(self->mod->typesec, aux_u32);
        if (*idx != 0x60) //TODO unroll this and validate function type
        {
            return 2; //invalid function type index
        }
        //add increment to import function count
        self->c.imports_functions_count++;
        return 0;        
    case 0x01:          //table idx
        //TODO
        return 0;
    case 0x02:          //mem idx
        return err_code = ValidateMemTypeBuf(import.external);        
    case 0x03:          //global idx
        idx = import.external;
        if(*idx != 0x00 || *idx != 0x01){
            idx++;  //skip mutability byte
        }
        return ValidateValType(idx);
    case 0x04:          //tag idx
        //TODO
        return 0;
    default:
        err_code = 99;
        return err_code;
    }
}

/**
 * @brief Implement global validation procedure according to webassembly spec.
 * @param self. Validator object
 * @param index.
 * @param global_section_end.
 */
uint32_t ValidateGlobal(WpValidatorState *self, const uint8_t *index, const uint8_t *global_section_end)
{

    WasmBinGlobal global;
    uint32_t err_code = ValidateGlobalBuf(index, global_section_end); // check for error on binary encoding
    if (err_code > 0)
    {
        return err_code;
    }

    // destructuring global
    global = DestructureGlobal(index);
    // validating value type
    if (IsNumericType(global.type))
    {
        // numeric types are allways valid
        err_code = WpValidateConstantExpresion(self, global.type[0], global.init_expr, global_section_end);
        if (err_code > 0)
        {
            return err_code;
        }
        return 0;
    }
    // vector type
    if (*global.type == 0x7B)
    {
        err_code = WpValidateConstantExpresion(self, global.type[0], global.init_expr, global_section_end);
        if (err_code > 0)
        {
            return err_code;
        }
        return 0;
    }

    if (IsRefType(global.type))
    {
        err_code = WpValidateConstantExpresion(self, global.type[0], global.init_expr, global_section_end);
        if (err_code > 0)
        {
            return err_code;
        }
        return 0;
    }

    return 100;
}

/**
 * @brief Implement export validation procedure according to webassembly spec.
 * @param seld. Validator object.
 * @param index.
 * @param export_section_end.
 */
uint32_t ValidateExport(WpValidatorState *self, const uint8_t *index, const uint8_t *export_section_end)
{

    WasmBinExport export;
    uint8_t *idx;
    uint32_t aux_idx;
    uint32_t error_code = ValidateExportBuf(index);
    if (error_code > 0)
    {
        return error_code;
    }

    // Destructuring export
    export = DestructureExport(index);
    switch (export.index_type)
    {
    case 0x00:          //func idx
        if(export.external_index < self->c.functions_count+self->c.imports_functions_count){
            //if external index exist should be valid due a function sectrion validation
            return 0;
        }
        return 99;        
    case 0x01:          //table idx
        //TODO
        return 99;
    case 0x02:          //mem idx
        if(export.external_index < self->c.mems_count){
            return 0;
        }
        return 99;
    case 0x03:          //global idx
        //TODO
        return 99;
    case 0x04:          //tag idx
        //TODO
        return 99;
    default:
        error_code = 99;
        return error_code;
    }
}

/**
 * @brief Main function for validator module. Implement validation procedure acording to webassembly Spec
 * @param self Validator object
 * @param mod Pointer to module to validate.
 * @return WpError object if fault or WpModule self if succes.
 * 
 */
WpObject *WpValidatorValidateModule(WpValidatorState *self, WpModuleState *mod)
{

    if (mod->buf == NULL)
    {
        self->err->id = 502;
        #ifdef WASPC_CONFIG_DEV_FLAG
        self->err->module_id = 2;
        self->err->func_code = 1;
        self->err->block_code = 1;
        #endif
        return (WpObject *)self->err;
    }

    if(self->value_stack == NULL){
        
        self->err->id = 503;    // NO MEMORY ALLOCATED FOR VALUE STACK
        #ifdef WASPC_CONFIG_DEV_FLAG
        self->err->module_id = 2;
        self->err->func_code = 1;
        self->err->block_code = 2;
        #endif
        return (WpObject *)self->err;        
    }
    //Init validator module
    self->mod = mod;
    
    const uint8_t *index = mod->buf;               // pointer to byte to traverse the binary file
    const uint8_t *buf_end = index + mod->bufsize; // pointer to end of binary module
    uint32_t decoded_u32 = 0;                      // auxiliary var to store u32 values
    uint8_t section_id;
    uint8_t last_loaded_section = 0; // var to keep track section order.
#define READ_BYTE() (*index++)
#define NOT_END() (index < buf_end)

    // Check magic number /////////////////////////////////////////////////////////////////////////
    if (ValidateMagicBuf(index) > 0)
    {
        self->err->id = 13;
        mod->status = WP_MODULE_STATUS_INVALID;
#if WASPC_CONFIG_DEV_FLAG == 1
        self->err->module_id = 2;
        self->err->func_code = 1;
        self->err->block_code = 3;
#endif
        return (WpObject *)&self->err;
    }
    index = index + 4;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Check version number /////////////////////////////////////////////////////////////////////////
    if (ValidateVersionBuf(index, &decoded_u32) > 0)
    {
        self->err->id = 14;
        mod->status = WP_MODULE_STATUS_INVALID;
#if WASPC_CONFIG_DEV_FLAG == 1
        self->err->module_id = 2;
        self->err->func_code = 1;
        self->err->block_code = 4;
#endif
        return (WpObject *)&self->err;
    }
    index = index + 4;
    mod->version = decoded_u32;
    ///////////////////////////////////////////////////////////////////////////////////////////////
    // Traversing the binary file
    while (NOT_END())
    {
        // Seccion
        section_id = READ_BYTE();
        index = ValidateBinSectionById(self, index, section_id, &last_loaded_section);
        if (!index)
        {
            self->err->id = 16;
#if WASPC_CONFIG_DEV_FLAG == 1
//keep settings from ValidateBinSectionById
#endif 
            mod->status = WP_MODULE_STATUS_INVALID;
            return (WpObject *)self->err;
        }
    }

#undef READ_BYTE
#undef NOT_END

    mod->status = WP_MODULE_STATUS_VALIDATED;
    return (WpObject *)mod;
}