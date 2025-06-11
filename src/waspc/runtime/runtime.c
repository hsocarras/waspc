/**
 * @file runtime.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2025-01-22
 * 
 * @copyright Copyright (c) 2025
 * 
 */

#include "runtime/runtime.h"
#include "objects/object.h"
#include "objects/export.h"
#include "validation/wasm_validator.h"
#include "webassembly/execution/runtime/values.h"
#include "runtime/alloc.h"


#include <assert.h>
#include <string.h>
#include <stdlib.h>
//#include <stdio.h>

/**
 * @brief Allocates and initializes a module instance in the runtime.
 *
 * This static function sets up the internal structures for a module instance,
 * including allocating function addresses for each function in the module and
 * creating export instances for all module exports. It links the exports to the
 * corresponding function, table, memory, or global instances as appropriate.
 *
 * @param self Pointer to the runtime state.
 * @param mod Pointer to the module state to allocate and instantiate.
 * @param externv Array of external values to satisfy the module's imports.
 * @param extern_len Number of external values in the array.
 * @return WpObject* Returns a pointer to the initialized module instance.
 */
static WpObject * WpRuntimeAllocateModule(WpRuntimeState *self, WpModuleState *mod, ExternalValue *externv, uint32_t extern_len){

    //step 2 For each function func𝑖 in module.funcs, do:
    //    a. Let funcaddr𝑖 be the function address resulting from allocating func𝑖 
    mod->instance.funcaddrs.lenght = 0;
    mod->instance.funcaddrs.elements = (funcaddr *)malloc(sizeof(funcaddr) * mod->was.funcs.lenght);
    for(uint32_t i = 0; i < mod->was.funcs.lenght; i++){
        mod->instance.funcaddrs.lenght ++;
        mod->instance.funcaddrs.elements[i] = WpAllocFunction(mod->was.funcs.elements[i], &mod->instance);
    }

    //step 18
    mod->instance.exports.elements = (WpExportInstance *)malloc(sizeof(WpExportInstance) * mod->was.exports.lenght);
    if(!mod->instance.exports.elements){
        self->err.id = 3;
        #if WASPC_CONFIG_DEV_FLAG == 1
        strcpy_s(self->err.file, 64,"runtime/runtime.c"); 
        strcpy_s(self->err.func, 32,"WpRuntimeLoadModule");
        #endif
        return (WpObject *)&self->err;
    }
    for(uint32_t i = 0; i < mod->was.exports.lenght; i++){
       
        //WpExportInstanceInit(export_instance);
        mod->instance.exports.elements[i].name.name = mod->was.exports.elements[i].name.name;
        mod->instance.exports.elements[i].name.lenght = mod->was.exports.elements[i].name.lenght;
        mod->instance.exports.elements[i].export_type = mod->was.exports.elements[i].type;
        switch (mod->was.exports.elements[i].type)
        {
        case WP_EXTERNAL_TYPE_FUNC:
            mod->instance.exports.elements[i].value.func = mod->instance.funcaddrs.elements[i];
            break;
        case WP_EXTERNAL_TYPE_TABLE:
            break;
        case WP_EXTERNAL_TYPE_MEMORY:
            break;
        case WP_EXTERNAL_TYPE_GLOBAL:
            break;
        default:
            break;
        }        
    }
    

    return (WpObject *)&mod->instance;

}
/**
 * @brief Init function for runtime state object
 * 
 * @param self 
 */
void WpRuntimeInit(WpRuntimeState *self){
    
    // Init code mem relate properties
    self->code_mem_start = NULL;
    self->code_mem_ptr = NULL;
    self->code_mem_size = 0;    

    // Init error object
    WpErrorInit(&self->err);

    //Init validator
    WpValidatorStateInit(&self->validator);

    // Init modules hash table
    HashTableInit(&self->modules);    
}

/**
 * @brief Function to init code memory area
 * 
 * @param self 
 * @param start pointer to where memory start
 * @param mem_size memory size in bytes
 */
void WpRuntimeCodeMemInit(WpRuntimeState *self, const uint8_t *start, uint32_t mem_size){

    // Init code memory
    self->code_mem_start = start;
    self->code_mem_ptr = (uint8_t *)start;
    self->code_mem_size = mem_size;
}


void WpRuntimetableInit(WpRuntimeState *self, HtEntry *table, uint32_t number_entries){

    HastTableSetup(&self->modules, table, number_entries);

}


/**
 * @brief Function to get the binary module into the runtime memory
 * 
 * @param self 
 * @param mod_name every module must have a name to be identified.
 * @param buffer_start address of the buffer where the module is stored.
 * @param mod_size module size in bytes.
 * @return WpObject* Error object if error, or module object if success.
 */
WpObject * WpRuntimeReadModule(WpRuntimeState *self, Name mod_name, const uint8_t *buffer_start, uint32_t mod_size){

    //first check that code memory is not null
    assert(self->code_mem_start);

    //check that hash table was initilised corectly.    
    if(self->modules.capacity == 0){
        self->err.id = 1;
        #if WASPC_CONFIG_DEV_FLAG == 1
        strcpy_s(self->err.file, 64,"runtime/runtime.c"); 
        strcpy_s(self->err.func, 32,"WpRuntimeLoadModule");
        #endif
        return (WpObject *)&self->err;
    }

    //check that are enought memory.
    uint32_t memory_usage = self->code_mem_ptr - self->code_mem_start;
    uint32_t memory_left = self->code_mem_size - memory_usage;
    if(mod_size > memory_left){
        self->err.id = 2;
        #if WASPC_CONFIG_DEV_FLAG == 1
        strcpy_s(self->err.file, 64,"runtime/runtime.c"); 
        strcpy_s(self->err.func, 32,"WpRuntimeLoadModule");
        #endif
        return (WpObject *)&self->err;
    }

    //copy module to code memory
    uint8_t *mod_start_address = memcpy(self->code_mem_ptr, buffer_start, mod_size);
    self->code_mem_ptr += mod_size;         //move pointer to next free memory
    
    //create module object
    WpModuleState *mod = (WpModuleState *)malloc(sizeof(WpModuleState));
    if(!mod){
        self->err.id = 3;
        #if WASPC_CONFIG_DEV_FLAG == 1
        strcpy_s(self->err.file, 64,"runtime/runtime.c"); 
        strcpy_s(self->err.func, 32,"WpRuntimeLoadModule");
        #endif
        return (WpObject *)&self->err;
    }
    WpModuleInit(mod);
    
    //register module object
    assert(mod_name.name);    
    HtEntry *curent_entry = HashTableSet(&self->modules, mod_name, mod);
    
    if(!curent_entry){
        self->err.id = 4;
        #if WASPC_CONFIG_DEV_FLAG == 1
        strcpy_s(self->err.file, 64,"runtime/runtime.c"); 
        strcpy_s(self->err.func, 32,"WpRuntimeLoadModule");
        #endif
        return (WpObject *)&self->err;
    }

    //copy module name
    
    mod->name = curent_entry->key;  //copy name from hash table entry
    //set module properties  
    mod->status = WP_MODULE_STATUS_READ;
    mod->buf = mod_start_address;
    mod->bufsize = mod_size;

    return (WpObject *)mod;

}

/**
 * @brief Validates a WebAssembly module loaded into the runtime.
 *
 * This function checks the integrity and correctness of a module's binary format,
 * including the magic number, version, and the order and validity of all sections.
 * If the module is valid, its status is updated to WP_MODULE_STATUS_VALIDATED.
 * If any validation step fails, an error object is returned and the module status
 * is set to WP_MODULE_STATUS_INVALID.
 *
 * @param self Pointer to the runtime state.
 * @param mod Pointer to the module state to validate.
 * @return WpObject* Returns the validated module object on success,
 *                   or an error object on failure.
 */
WpObject * WpRuntimeValidateModule(WpRuntimeState *self, WpModuleState *mod){ 
    
    if(!mod){
        self->err.id = 11;
        #if WASPC_CONFIG_DEV_FLAG == 1
        strcpy_s(self->err.file, 64,"runtime/runtime.c"); 
        strcpy_s(self->err.func, 32,"WpRuntimeValidateModule");
        #endif
        return (WpObject *)&self->err;
    }
        
    
    const uint8_t *index = mod->buf;                                // pointer to byte to traverse the binary file    
    const uint8_t *buf_end = index + mod->bufsize;                  // pointer to end of binary module
    uint32_t decoded_u32 = 0;                                       // auxiliary var to store u32 values
    uint8_t section_id;
    uint8_t last_loaded_section = 0;                                // var to keep track section order.

    #define READ_BYTE() (*index++)
    #define NOT_END() (index < buf_end)

    // Check minimun module size for magic and version number ///////////////////////////////////
    if(mod->bufsize < 12){
        self->err.id = 5;
        mod->status = WP_MODULE_STATUS_INVALID;
        #if WASPC_CONFIG_DEV_FLAG == 1
        strcpy_s(self->err.file, 64,"runtime/runtime.c"); 
        strcpy_s(self->err.func, 32,"WpRuntimeValidateModule");
        #endif
        return (WpObject *)&self->err;
    }

    // Check magic number /////////////////////////////////////////////////////////////////////////    
    if (!ValidateMagic(index)){
        self->err.id = 13;
        mod->status = WP_MODULE_STATUS_INVALID;
        #if WASPC_CONFIG_DEV_FLAG == 1
        strcpy_s(self->err.file, 64,"runtime/runtime.c"); 
        strcpy_s(self->err.func, 32,"WpRuntimeValidateModule");
        #endif
        return (WpObject *)&self->err;
    }   
    index = index + 4; 
    ///////////////////////////////////////////////////////////////////////////////////////////////

    // Check version number /////////////////////////////////////////////////////////////////////////    
    if (!ValidateVersion(index, &decoded_u32)){
        self->err.id = 14;
        mod->status = WP_MODULE_STATUS_INVALID;
        #if WASPC_CONFIG_DEV_FLAG == 1
        strcpy_s(self->err.file, 64,"runtime/runtime.c"); 
        strcpy_s(self->err.func, 32,"WpRuntimeValidateModule");
        #endif
        return (WpObject *)&self->err;    
    }
    index = index + 4;    
    mod->version = decoded_u32;
    ///////////////////////////////////////////////////////////////////////////////////////////////

    //Traversing the binary file 
    while(NOT_END()){
        //Seccion
        section_id = READ_BYTE();        
        //
        index = ValidateBinSectionById(index, section_id, &last_loaded_section, mod);
        if (!index){  
            self->err.id = 16;
            mod->status = WP_MODULE_STATUS_INVALID;
            #if WASPC_CONFIG_DEV_FLAG == 1
            strcpy_s(self->err.file, 64,"runtime/runtime.c"); 
            strcpy_s(self->err.func, 32,"WpRuntimeValidateModule");
            #endif
            return (WpObject *)&self->err;                                                             
        }        
    }
    
   

    #undef READ_BYTE
    #undef NOT_END

    mod->status = WP_MODULE_STATUS_VALIDATED;
    return (WpObject *)mod;
}

/**
 * @brief Instantiates a validated WebAssembly module within the runtime.
 *
 * This function creates a module instance from a validated module, resolving all imports
 * using the provided array of external values. It checks that the module is validated and
 * that the number of provided external values matches the number of imports required.
 * On success, the module is instantiated and ready for execution.
 * On failure, an error object is returned.
 *
 * @param self Pointer to the runtime state.
 * @param mod Pointer to the validated module state to instantiate.
 * @param externv Array of external values to satisfy the module's imports.
 * @param extern_len Number of external values in the array.
 * @return WpObject* Returns the instantiated module object on success,
 *                   or an error object on failure.
 */
WpObject * WpRuntimeInstanciateModule(WpRuntimeState *self, WpModuleState *mod, ExternalValue *externv, uint32_t extern_len){

    if(!mod){
        self->err.id = 21;
        #if WASPC_CONFIG_DEV_FLAG == 1
        strcpy_s(self->err.file, 64,"runtime/runtime.c"); 
        strcpy_s(self->err.func, 32,"WpRuntimeInstanciateModule");
        #endif
        return (WpObject *)&self->err;
    }

    //1 If module is not valid, then:
    //  a. Fail
    if(mod->status != WP_MODULE_STATUS_VALIDATED){
        self->err.id = 22;
        #if WASPC_CONFIG_DEV_FLAG == 1
        strcpy_s(self->err.file, 64,"runtime/runtime.c"); 
        strcpy_s(self->err.func, 32,"WpRuntimeInstanciateModule");
        #endif
        return (WpObject *)&self->err;
    }

    //3 -If the number 𝑚 of imports is not equal to the number 𝑛 of provided external values, then: Fail
    if(mod->was.imports.lenght != extern_len){
        self->err.id = 24;
        #if WASPC_CONFIG_DEV_FLAG == 1
        strcpy_s(self->err.file, 64,"runtime/runtime.c"); 
        strcpy_s(self->err.func, 32,"WpRuntimeInstanciateModule");
        #endif
        return (WpObject *)&self->err;

    }

    ExternalValue ext;

    if(externv){
        //4-For each external value 𝑒𝑖 in the provided external values:
        for(uint32_t i=0; i < extern_len; i++){
            ExternalValue ext = externv[i];
            switch (ext.type)
            {
            case WP_EXTERNAL_TYPE_FUNC:
                //Assert: ext is a function
                //1. If the function is not valid, then:
                //  a. Fail
                
                break;
            case WP_EXTERNAL_TYPE_TABLE:
                //Assert: ext is a table
                break;
            case WP_EXTERNAL_TYPE_MEMORY:
                //Assert: ext is a memory
                break;
            case WP_EXTERNAL_TYPE_GLOBAL:
                //Assert: ext is a global
                break;
            default:
                break;
            }
        }


    }

    // Initialize module instance
    WpModuleInstance aux;
    WpModuleInstanceInit(&aux);

    //11
    WpObject *result = WpRuntimeAllocateModule(self, mod, externv, extern_len);

    return (WpObject *)&mod->instance;

}

/**
 * @brief Invokes the main function of an instantiated WebAssembly module.
 *
 * This function searches for the "main" export in the given module and, if found and valid,
 * invokes it with no argument. The module must be in the instantiated state. If the main
 * function is missing or not a function export, or if the module is not instantiated,
 * an error object is returned. Is a shorcut for calling WpFuncInstanceInvoke with the main function address.
 *
 * @param self Pointer to the runtime state.
 * @param mod Pointer to the instantiated module.
 * @return WpObject* Returns the result of the main function invocation on success,
 *                   or an error object on failure.
 */
WpObject * WpRuntimeInvocateProgram(WpRuntimeState *self, WpModuleInstance *m_instance){

    Name main_func = {5, "main"};

    
    //2-If the module does not have a main function, then: Return
    
    for(uint32_t i = 0; i < m_instance->exports.lenght; i++){
        if(strncmp(m_instance->exports.elements[i].name.name, main_func.name, 5) == 0){

            //3-If the main function is not valid, then: Fail
            if(m_instance->exports.elements[i].export_type != WP_EXTERNAL_TYPE_FUNC){
                self->err.id = 32;
                #if WASPC_CONFIG_DEV_FLAG == 1
                strcpy_s(self->err.file, 64,"runtime/runtime.c"); 
                strcpy_s(self->err.func, 32,"WpRuntimeInvocateProgram");
                #endif
                return (WpObject *)&self->err;
            }

            //4-Invoke the main function
           funcaddr main = m_instance->exports.elements[i].value.func;

            return WpFuncInstanceInvoke(self, main, NULL, 0);
        }
    }

    //3-If the main function is not found, then: Return Error    
    self->err.id = 32;
    #if WASPC_CONFIG_DEV_FLAG == 1
    strcpy_s(self->err.file, 64,"runtime/runtime.c"); 
    strcpy_s(self->err.func, 32,"WpRuntimeInvocateProgram");
    #endif
    return (WpObject *)&self->err;
    

}

/**
 * @brief Invokes a WebAssembly function instance according to the WebAssembly specification.
 *
 * This function sets up a new activation frame, initializes local variables (parameters + locals)
 * using the provided argument values, and executes the function body. It manages the operand stack
 * and handles the function's return value(s). If an error occurs during invocation, an error object is returned.
 *
 * @param self Pointer to the runtime state.
 * @param func Pointer to the function instance to invoke.
 * @param args Pointer to an array of argument values to pass to the function (parameters).
 * @param argc Number of argument values in the args array.
 * @return WpObject* Returns the result of the function invocation on success,
 *                   or an error object on failure.
 */
WpObject * WpFuncInstanceInvoke(WpRuntimeState *self, funcaddr func, Value *args, uint32_t argc) {
    
    if (!func) {
        self->err.id = 40;
        #if WASPC_CONFIG_DEV_FLAG == 1
        strcpy_s(self->err.file, 64, "runtime/runtime.c");
        strcpy_s(self->err.func, 32, "WpFuncInstanceInvoke");
        #endif
        return (WpObject *)&self->err;
    }

    // 1. Allocate and initialize a new activation frame
    ActivationFrame frame;
    WpFunctionInstance *f_inst = (WpFunctionInstance *)func;
    //frame.arity = f_inst->arity ? *(f_inst->arity) : 0;
    frame.module = f_inst->module;
    frame.func = f_inst;
    //frame.locals_count = f_inst->code->locals_count;

    // 2. Allocate and initialize locals (params + locals)
    frame.locals = (Value *)malloc(sizeof(Value) * frame.locals_count);
    if (!frame.locals) {
        self->err.id = 41;
        #if WASPC_CONFIG_DEV_FLAG == 1
        strcpy_s(self->err.file, 64, "runtime/runtime.c");
        strcpy_s(self->err.func, 32, "WpFuncInstanceInvoke");
        #endif
        return (WpObject *)&self->err;
    }

    // Copy arguments (parameters) to locals
    uint32_t param_count = argc;
    if (param_count > frame.locals_count) param_count = frame.locals_count;
    for (uint32_t i = 0; i < param_count; i++) {
        frame.locals[i] = args[i];
    }
    // Initialize remaining locals to zero
    for (uint32_t i = param_count; i < frame.locals_count; i++) {
        frame.locals[i] = (Value){0};
    }

    // 3. Push the frame to the call stack (not shown, depends on your runtime)
    // push_activation_frame(self, &frame);

    // 4. Execute the function body (interpreter or JIT)
    WpObject *result;// = WpExecuteFunctionBody(self, &frame);

    // 5. Pop the frame from the call stack (not shown)
    // pop_activation_frame(self);

    // 6. Free locals
    free(frame.locals);

    return result;
}
