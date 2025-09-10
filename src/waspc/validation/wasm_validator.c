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
//#include <stdio.h>



/**
 * @brief Inner function to read a binary section into a WasmBinSection structure 
 * 
 * @param index 
 * @param sec 
 * @return const uint8_t* index pointing at next section or NULL if error
 */
static const uint8_t * ReadBinSection(const uint8_t *index, WasmBinSection *sec){

    uint32_t aux_u32;
    index = DecodeLeb128UInt32(index, &aux_u32);
    if (!index){
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
void WpValidatorStateInit(WpValidatorState *self){

    // Initialize the validator state context c
    //self->c.types.lenght = 0;
    //self->c.types.elements = NULL;
    self->c.locals.lenght = 0;
    self->c.locals.elements = NULL;
    self->c.return_types.lenght = 0;
    self->c.return_types.elements = NULL;

    // Initialize the prime context
    //self->c_prime.types.lenght = 0;
    //self->c_prime.types.elements = NULL;
    self->c_prime.locals.lenght = 0;
    self->c_prime.locals.elements = NULL;
    self->c_prime.return_types.lenght = 0;
    self->c_prime.return_types.elements = NULL;

    self->val_stack = NULL;
    self->stk_ptr = NULL;
    self->val_stack_size = 0; // Set default stack size

    self->ctrl_stack = NULL;
    self->ctr_stack_idx = 0; // Initialize control stack index
    self->ctr_stack_size = 0; // Set default control stack size

    self->ip = NULL; // Initialize instruction pointer to NULL
}


//todo
uint8_t WpValidateExpr(WpValidatorState *self, Context *context, const Expr *e) {
    /*
    // Validate the expression
    if (!WpValidateConstantExpr(self, e, expected_types, expected_types_len)) {
        return 0; // Invalid expression
    }

    // Check if the expression ends with 0x0B
    if (*(e->end - 1) != 0x0B) {
        return 0; // Expression does not end with 0x0B
    }
    */
    return 1; // Expression is valid
}

/**
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
 * Checks if the section is in the correct order and validates each section.
 * @param self Pointer to the validator state
 * @param index 
 * @param section_id 
 * @param previous_secction 
 * @param mod 
 * @return const uint8_t* pointer to next section or NULL if error
 */
const uint8_t* ValidateBinSectionById(WpValidatorState *self, const uint8_t *index, const uint8_t section_id, uint8_t *previous_secction, WpModuleState *mod){

    uint32_t aux_u32;                                               //auxiliary var to store u32 values 
    uint32_t len;
    uint32_t i;

    /**
     * @brief Sections inside a module are encoded in a specific order.
     * this switch case is to validate the order of the sections.
     */
    switch(*previous_secction){
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
    const uint8_t *functype_index;      
    if(section_id == WP_WSA_BIN_MOD_SEC_ID_TYPE){
        // Type Section 
        index = ReadBinSection(index, &mod->typesec);        
        if (!index){  
            mod->status = WP_MODULE_STATUS_ERROR;              
            return NULL;                                        
        }
        
        functype_index = mod->typesec.content; // Pointer to the start of the type section content
        functype_index = DecodeLeb128UInt32(functype_index, &len); // Get the number of types in the section
        if (!functype_index){
            mod->status = WP_MODULE_STATUS_ERROR; 
            return NULL; // No types in the section
        }        
        mod->functype_count = len; // Store the number of function types in the module

        for (i = 0; i < len; i++)
        {
            functype_index = GetFuncTypeByIndex(mod->typesec, i);
            if (!functype_index) {
                mod->status = WP_MODULE_STATUS_ERROR; 
                return NULL; // Invalid function type
            }
            if (!ValidateFuncTypeBuf(functype_index)) {
                mod->status = WP_MODULE_STATUS_INVALID; 
                return NULL; // Invalid function type
            }
        }  
               
        // Set the context types
        self->c.functypes_count = mod->functype_count;
        self->c.types = mod->typesec.content; // Pointer to the types section content
        // previous section id is now type section
        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_TYPE;
        return index;
    }

    start_at_import_sec:
    const uint8_t *import_index;
    if(section_id ==  WP_WSA_BIN_MOD_SEC_ID_IMPORT){
        // import Section 
        index = ReadBinSection(index, &mod->importsec);
        if (!index){  
            mod->status = WP_MODULE_STATUS_ERROR;               
            return NULL;                                        
        }
        import_index = mod->importsec.content; // Pointer to the start of the import section content
        import_index = DecodeLeb128UInt32(import_index, &len); // Get the number of imports in the section
        if (!import_index){
            mod->status = WP_MODULE_STATUS_ERROR; 
            return NULL; // No imports in the section
        }   
        mod->import_count = len; // Store the number of imports in the module

        for (i = 0; i < len; i++)
        {
            import_index = GetImportByIndex(mod->importsec, i);
            if (!import_index) {
                mod->status = WP_MODULE_STATUS_ERROR; 
                return NULL; // Invalid import
            }
            //Validate import
            if(!ValidateImportBuf(import_index, mod->functype_count)) {
                mod->status = WP_MODULE_STATUS_INVALID; 
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
    if(section_id ==  WP_WSA_BIN_MOD_SEC_ID_FUNCTION){
        // Function Section 
        index = ReadBinSection(index, &mod->functionsec);
        if (!index){ 
            mod->status = WP_MODULE_STATUS_ERROR;               
            return NULL;                                        
        }
        
        function_index = mod->functionsec.content; // Pointer to the start of the function section content
        function_index = DecodeLeb128UInt32(function_index, &len); // Get the number of functions in the section
        if (!function_index){
            mod->status = WP_MODULE_STATUS_ERROR; 
            return NULL; // No functions in the section
        }
        mod->function_count = len; // Store the number of functions in the module
        for(i = 0; i < len; i++){
            function_index = GetFunctionByIndex(mod->functionsec, i);
            if(!function_index){
                mod->status = WP_MODULE_STATUS_ERROR; 
                return NULL; // Invalid function
            }
            //Validate function
            function_index = DecodeLeb128UInt32(function_index, &function_type_index); // Get the function type index
            if(!function_index){
                mod->status = WP_MODULE_STATUS_ERROR; 
                return NULL; // Invalid function type index
            }
            if(function_type_index >= mod->functype_count){
                mod->status = WP_MODULE_STATUS_INVALID; 
                return NULL; // Invalid function type index
            }
        }
        // previous section id is now function section
        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_FUNCTION;
        return index;
    }

    start_at_table_sec:   
    const uint8_t *table_index;    
    if(section_id == WP_WSA_BIN_MOD_SEC_ID_TABLE){
        // Table Section
        index = ReadBinSection(index, &mod->tablesec);
        if (!index){           
            mod->status = WP_MODULE_STATUS_ERROR;      
            return NULL;                                        
        }   
        
        table_index = mod->tablesec.content; // Pointer to the start of the table section content
        table_index = DecodeLeb128UInt32(table_index, &len); // Get the number of tables in the section
        if (!table_index){
            mod->status = WP_MODULE_STATUS_ERROR; 
            return NULL; // No tables in the section
        }
        mod->table_count = len; // Store the number of tables in the module 

        /// The table type tabletype must be valid.
        for(i = 0; i < len; i++){
            table_index = GetTableByIndex(mod->tablesec, i);
            if(!table_index){
                mod->status = WP_MODULE_STATUS_ERROR; 
                return NULL; // Invalid table
            }
            
            if(!ValidateTableTypeBuf(table_index)){
                mod->status = WP_MODULE_STATUS_INVALID;
                //Invalid table limits
                return NULL;
            }
        }

        // previous section id is now table section
        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_TABLE;
        return index;
    }

    start_at_memory_sec:
    const uint8_t *memory_index;
    if(section_id == WP_WSA_BIN_MOD_SEC_ID_MEMORY){
        // Memory Section
        index = ReadBinSection(index, &mod->memsec);
        if (!index){        
            mod->status = WP_MODULE_STATUS_ERROR;         
            return NULL;                                        
        } 
        
        memory_index = mod->memsec.content; // Pointer to the start of the memory section content
        memory_index = DecodeLeb128UInt32(memory_index, &len); // Get the number of memories in the section
        if (!memory_index){
            mod->status = WP_MODULE_STATUS_ERROR; 
            return NULL; // No memories in the section
        }
        mod->memory_count = len; // Store the number of memories in the module

        /// The memory type memtype must be valid.
        for(i = 0; i < len; i++){
            memory_index = GetMemByIndex(mod->memsec, i);
            if(!memory_index){
                mod->status = WP_MODULE_STATUS_ERROR; 
                return NULL; // Invalid memory
            }
            //Validate memory type
            if(!ValidateLimitsTypeBuf(memory_index, 0x10000)){
                mod->status = WP_MODULE_STATUS_INVALID;
                //Invalid memory limits
                return NULL;
            }
        }

        // previous section id is now memory section
        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_MEMORY;
        return index;
    }

    start_at_global_sec:
    const uint8_t *global_index;
    ValType gt;
    if(section_id == WP_WSA_BIN_MOD_SEC_ID_GLOBAL){
        // GLOBAL Section
        index = ReadBinSection(index, &mod->globalsec);
        if (!index){    
            mod->status = WP_MODULE_STATUS_ERROR;             
            return NULL;                                        
        } 
        global_index = mod->globalsec.content; // Pointer to the start of the global section content
        global_index = DecodeLeb128UInt32(global_index, &len); // Get the number of globals in the section
        if (!global_index){
            mod->status = WP_MODULE_STATUS_ERROR; 
            return NULL; // No globals in the section
        }
        mod->global_count = len; // Store the number of globals in the module

        for(i = 0; i < len; i++){
            /// The global type must be valid.
            global_index = GetGlobalByIndex(mod->globalsec, i);
            if(!global_index){
                mod->status = WP_MODULE_STATUS_ERROR; 
                return NULL; // Invalid global
            }
            gt =  *global_index;  // Get global type value type
            if(!ValidateGlobalTypeBuf(global_index)){
                mod->status = WP_MODULE_STATUS_INVALID; 
                return NULL; // Invalid global type
            }
            //Validate global init expr   
            global_index += 2;           // Move pointer to the start of the init expr         
            if(!ValidateConstantExprBuf(global_index, 0xFFFFFFFF)){
                mod->status = WP_MODULE_STATUS_INVALID; 
                return NULL; // Invalid global type
            }
        }
        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_GLOBAL;
        return index;
    }

    start_at_export_sec:
    const uint8_t *export_index;
    if(section_id == WP_WSA_BIN_MOD_SEC_ID_EXPORT){
        //Export Section                
        index = ReadBinSection(index, &mod->exportsec);
        if (!index){       
            mod->status = WP_MODULE_STATUS_ERROR;          
            return NULL;                                        
        } 
        export_index = mod->exportsec.content; // Pointer to the start of the export section content
        export_index = DecodeLeb128UInt32(export_index, &len); // Get the number of exports in the section
        if (!export_index){
            mod->status = WP_MODULE_STATUS_ERROR; 
            return NULL; // No exports in the section
        }
        mod->export_count = len; // Store the number of exports in the module

        for(i = 0; i < len; i++){
            export_index = GetExportByIndex(mod->exportsec, i);
            if(!export_index){
                mod->status = WP_MODULE_STATUS_ERROR; 
                return NULL; // Invalid export
            }
            //Validate export
            if(!ValidateExportBuf(export_index, mod->function_count, mod->table_count, mod->memory_count, mod->global_count)) {
                mod->status = WP_MODULE_STATUS_INVALID; 
                return NULL; // Invalid export
            }
        }
        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_EXPORT;
        return index;   
    }

    start_at_start_sec:
    const uint8_t *start_index;
    if(section_id == WP_WSA_BIN_MOD_SEC_ID_START){
        // Start Section
        index = ReadBinSection(index, &mod->startsec);
        if (!index){     
            mod->status = WP_MODULE_STATUS_ERROR;            
            return NULL;                                        
        } 
        index = DecodeLeb128UInt32(index, &mod->start); // Get the start function index
        if(!index){
            mod->status = WP_MODULE_STATUS_ERROR; 
            return NULL;
        }
        // The start function index must refer to a function in the module.
        if(mod->start >= mod->function_count){
            mod->status = WP_MODULE_STATUS_INVALID; 
            return NULL; // Invalid start function index
        }

        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_START;
        return index;  
    }

    start_at_element_sec:
    const uint8_t *element_index;
    if(section_id == WP_WSA_BIN_MOD_SEC_ID_ELEMENT){
        // Element Section              
        index = ReadBinSection(index, &mod->elemsec);
        if (!index){      
            mod->status = WP_MODULE_STATUS_ERROR;           
            return NULL;                                        
        } 
        element_index = mod->elemsec.content; // Pointer to the start of the element section content
        element_index = DecodeLeb128UInt32(element_index, &len); // Get the number of elements in the section
        if (!element_index){
            mod->status = WP_MODULE_STATUS_ERROR; 
            return NULL; // No elements in the section
        }
        mod->element_count = len; // Store the number of elements in the module
        for(i = 0; i < len; i++){
            element_index = GetElementByIndex(mod->elemsec, i);
            if(!element_index){
                mod->status = WP_MODULE_STATUS_ERROR; 
                return NULL; // Invalid element
            }
            //Validate element
            if(!ValidateElementBuf(element_index, mod->function_count, mod->table_count)) {
                mod->status = WP_MODULE_STATUS_INVALID; 
                return NULL; // Invalid element
            }
        }

        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_ELEMENT;
        return index;      
    }

    start_at_data_count_sec:
    const uint8_t *data_count_index;
    if(section_id == WP_WSA_BIN_MOD_SEC_ID_DATA_COUNT){
        //Data Count Section   
        index = ReadBinSection(index, &mod->datacountsec);
        if (!index){    
            mod->status = WP_MODULE_STATUS_ERROR;             
            return NULL;                                        
        }
        data_count_index = mod->datacountsec.content;                               // Pointer to the start of the data count section content
        data_count_index = DecodeLeb128UInt32(data_count_index, &mod->data_count);  // Get the number of data segments in the section
        if (!data_count_index){
            mod->status = WP_MODULE_STATUS_ERROR; 
            return NULL; // No data count in the section
        }
        
        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_DATA_COUNT;
        return index;
    }

    start_at_code_sec:  
    const uint8_t *code_index;  
    if(section_id == WP_WSA_BIN_MOD_SEC_ID_CODE){
        //Code Section   
        index = ReadBinSection(index, &mod->codesec);
        if (!index){                
            return NULL;                                        
        } 
        code_index = mod->codesec.content; // Pointer to the start of the code section content
        code_index = DecodeLeb128UInt32(code_index, &len); // Get the number of code in the section
        if (!code_index){
            mod->status = WP_MODULE_STATUS_ERROR; 
            return NULL; // No code in the section
        }
        if(len != mod->function_count){
            mod->status = WP_MODULE_STATUS_INVALID; 
            return NULL; // The number of code entries must match the number of functions
        }

        for(i = 0; i < len; i++){
            code_index = GetCodeByIndex(mod->codesec, i);
            if(!code_index){
                mod->status = WP_MODULE_STATUS_ERROR; 
                return NULL; // Invalid code
            }
            //Validate function
            if(!ValidateCodeBuf(code_index)) {
                mod->status = WP_MODULE_STATUS_INVALID; 
                return NULL; // Invalid function
            }
        }

        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_CODE;
        return index;
    }

    start_at_data_sec:
    const uint8_t *data_index;
    if(section_id == WP_WSA_BIN_MOD_SEC_ID_DATA){
        //Data Section 
        index = ReadBinSection(index, &mod->datasec);
        if (!index){     
            mod->status = WP_MODULE_STATUS_ERROR;            
            return NULL;                                        
        } 
        data_index = mod->datasec.content; // Pointer to the start of the data section content
        data_index = DecodeLeb128UInt32(data_index, &len); // Get the number of data segments in the section
        if (!data_index){
            mod->status = WP_MODULE_STATUS_ERROR; 
            return NULL; // No data segments in the section
        }
        if(len != mod->data_count){
            mod->status = WP_MODULE_STATUS_INVALID; 
            return NULL; // The number of data segments must match the data count
        }
        
        for(i = 0; i < len; i++){
            data_index = GetDataByIndex(mod->datasec, i);
            if(!data_index){
                mod->status = WP_MODULE_STATUS_ERROR; 
                return NULL; // Invalid data segment
            }
            //Validate data segment
            if(!ValidateDataBuf(data_index, mod->memory_count)) {
                mod->status = WP_MODULE_STATUS_INVALID; 
                return NULL; // Invalid data segment
            }
        }
        
        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_DATA;
        return index;    
    }

    start_at_custom_sec:    
    if(section_id == WP_WSA_BIN_MOD_SEC_ID_CUSTOM){                
        //custom section will be ignored 
        // Has no use on module execution.
        index = DecodeLeb128UInt32(index, &aux_u32);
        if (!index){      
            mod->status = WP_MODULE_STATUS_ERROR;           
            return NULL;                                        
        } 
        index = index + aux_u32;
        *previous_secction = WP_WSA_BIN_MOD_SEC_ID_CUSTOM;
        return index;
    }

    return NULL;
}

/*To clean out

static uint8_t ValidateTableType(const TableType *t){    

    return ValidateLimitsType(&t->lim, 0xFFFFFFFF);
}

static uint8_t ValidateLimitsType(const Limits *li, uint32_t k){    

    if(li->min >= k){
        if(li->max >0){
            if(li->max <= k & li->max >= li->min){
                //valid
                return 1;
            }
            else{
                //not valid max value
                return 0;
            }

        }        
    }
    else{
        //not valid min value        
        return 0;
    }
    return 0;
}

static uint8_t ValidateTableType(const TableType *t){    

    return ValidateLimitsType(&t->lim, 0xFFFFFFFF);
}

static uint8_t ValidateMemType(const MemType *m){    

    return ValidateLimitsType(m, 0x010000); // 2^16 = 65536
}

uint8_t ValidateGlobalType(WpValidatorState *self, const Global *g){
    
    GlobalType gt = g->gt;
    // Validate global type
    if(!ValidateValType(gt.t)){
        //Invalid global type
        return 0;
    }
    if(gt.m != 0 && gt.m != 1){
        //Invalid global mutability
        return 0;
    }
    
    if(!WpValidateConstantExpr(self,&g->e, &gt.t, 1)){
        //Invalid constant expression
        return 0;

    }

    return 1;
}

static WpObjectResult ValidateImportSection(Import *imports, uint32_t imports_len){

    WpObjectResult result;
    InitObjectResult(&result);

    ImportDesc desc;

    for(int i=0; i<imports_len; i++){

        //first get import mod_name to get the context

        
        desc = imports[i].desc;

        switch(desc->type){

            case WP_WAS_STRUC_MOD_IMPORT_DESC_TYPE_FUNC:
                if(desc->idx < c.type_len){

                }
                else{
                    
                }
                break;
            case WP_WAS_STRUC_MOD_IMPORT_DESC_TYPE_TABLE:
                break;
            case WP_WAS_STRUC_MOD_IMPORT_DESC_TYPE_MEM:
                break
            case WP_WAS_STRUC_MOD_IMPORT_DESC_TYPE_GLOBAL:
                break;
            default:

        }
    }

}
*/