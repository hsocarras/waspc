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
#include "runtime/store.h"
#include "objects/object.h"
#include "objects/export.h"
#include "validation/wasm_validator.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>
// #include <stdio.h>


/**
 * @brief Init function for runtime state object
 *
 * @param self
 */
void WpRuntimeInit(WpRuntimeState *self)
{

    // Init error object
    WpErrorInit(&self->err);

    //Init store
    WpStoreInit(&self->store); //
    // HashTableInit(&self->modules);

    // Interpreter
    //WpInterpreterInit(&self->interpreter);

    // Init validator
    WpValidatorStateInit(&self->validator);
}

WpObject *WpRuntimeCreateModuleFromBinFile(WpRuntimeState *self, WpModuleState *mod_state, WpBinFile bin_file, Name mod_name)
{
    if (!mod_state)
    {
        return (WpObject *)&(self->err);
    }
    mod_state->wp_type = WP_OBJECT_MODULE_STATE;
    // mod_state->name = mod_name;
    mod_state->status = WP_MODULE_STATUS_INIT;
    mod_state->buf = bin_file.buf;
    mod_state->bufsize = bin_file.bufsize;
    return (WpObject *)mod_state;
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
WpObject *WpRuntimeValidateModule(WpRuntimeState *self, WpModuleState *mod)
{

    if (!mod)
    {
        self->err.id = 11;
#if WASPC_CONFIG_DEV_FLAG == 1
        strcpy_s(self->err.file, 64, "runtime/runtime.c");
        strcpy_s(self->err.func, 32, "WpRuntimeValidateModule");
#endif
        return (WpObject *)&self->err;
    }

    const uint8_t *index = mod->buf;               // pointer to byte to traverse the binary file
    const uint8_t *buf_end = index + mod->bufsize; // pointer to end of binary module
    uint32_t decoded_u32 = 0;                      // auxiliary var to store u32 values
    uint8_t section_id;
    uint8_t last_loaded_section = 0; // var to keep track section order.

#define READ_BYTE() (*index++)
#define NOT_END() (index < buf_end)

    // Check minimun module size for magic and version number ///////////////////////////////////
    if (mod->bufsize < 12)
    {
        self->err.id = 5;
        mod->status = WP_MODULE_STATUS_INVALID;
#if WASPC_CONFIG_DEV_FLAG == 1
        strcpy_s(self->err.file, 64, "runtime/runtime.c");
        strcpy_s(self->err.func, 32, "WpRuntimeValidateModule");
#endif
        return (WpObject *)&self->err;
    }

    // Check magic number /////////////////////////////////////////////////////////////////////////
    if (ValidateMagicBuf(index)> 0)    {
        self->err.id = 13;
        mod->status = WP_MODULE_STATUS_INVALID;
#if WASPC_CONFIG_DEV_FLAG == 1
        strcpy_s(self->err.file, 64, "runtime/runtime.c");
        strcpy_s(self->err.func, 32, "WpRuntimeValidateModule");
#endif
        return (WpObject *)&self->err;
    }
    index = index + 4;
    ///////////////////////////////////////////////////////////////////////////////////////////////

    // Check version number /////////////////////////////////////////////////////////////////////////
    if (ValidateVersionBuf(index, &decoded_u32) > 0){
        self->err.id = 14;
        mod->status = WP_MODULE_STATUS_INVALID;
#if WASPC_CONFIG_DEV_FLAG == 1
        strcpy_s(self->err.file, 64, "runtime/runtime.c");
        strcpy_s(self->err.func, 32, "WpRuntimeValidateModule");
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
        //
        index = ValidateBinSectionById(&self->validator, index, section_id, &last_loaded_section, mod);
        if (!index)
        {
            self->err.id = 16;
            mod->status = WP_MODULE_STATUS_INVALID;
#if WASPC_CONFIG_DEV_FLAG == 1
            strcpy_s(self->err.file, 64, "runtime/runtime.c");
            strcpy_s(self->err.func, 32, "WpRuntimeValidateModule");
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
WpObject *WpRuntimeInstanciateModule(WpRuntimeState *self, WpModuleState *mod, void *externv, uint32_t extern_len)
{
    const uint8_t *address;
    uint32_t u32_data;
    size_t i;

    if(!mod){
        self->err.id = 21;
        #if WASPC_CONFIG_DEV_FLAG == 1
        strcpy_s(self->err.file, 64,"runtime/runtime.c");
        strcpy_s(self->err.func, 32,"WpRuntimeInstanciateModule");
        #endif
        return (WpObject *)&self->err;
    }
    
    ///1 If module is not valid, then://///////////////////////////////////////////////////////////////////////////////
    //  a. Fail
    if(mod->status != WP_MODULE_STATUS_VALIDATED){
        WpObject *result = WpRuntimeValidateModule(self, mod);
        if(!result){
            return (WpObject *)&self->err;
        }
        if(result->wp_type == WP_OBJECT_ERROR){
            return (WpObject *)&self->err;
        }
        if(result->wp_type == WP_OBJECT_MODULE_STATE){
            if(((WpModuleState *)result)->status != WP_MODULE_STATUS_VALIDATED){
                return (WpObject *)&self->err;
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    ///3 

    for (i = 0; i < mod->type_count; i++)
    {
        address = GetTypeByIndex(mod->typesec, i);        
        if (!address)
        {
            self->err.id = 22;        
            return (WpObject *)&self->err;
        }
        if(i == 0){
            mod->types = WpStoreAllocTypes(&self->store, address);
            if (!mod->types)
            {
                self->err.id = 23;        
                return (WpObject *)&self->err;
            }
        }
        else{
            if(!WpStoreAllocTypes(&self->store, address)){
                self->err.id = 23;        
                return (WpObject *)&self->err;
            }
        }
    }
    
    /// 11, 12, 13 y 19 /////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Globals
     WasmBinGlobal global;
    for(i = 0; i < mod->global_count; i++){

        address = GetGlobalByIndex(mod->globalsec, i);
        if(!address){
            self->err.id = 24;        
            return (WpObject *)&self->err;
        }
        /// destructuring global
        global = DestructureGlobal(address);
        WasValue val = WpInterpreterEvalExpr(&self->interpreter, global.init_expr);
        if(val.type == WAS_EX_VAL_TYPE_NULL){
            self->err.id = 26;        
            return (WpObject *)&self->err;
        }
        if(i == 0){
            mod->globals = WpStoreAllocGlobal(&self->store, global.mut, global.type, val);
            if(!mod->globals){
                self->err.id = 25;        
                return (WpObject *)&self->err;
            }
        }
        else{
            if(!WpStoreAllocGlobal(&self->store, global.mut, global.type, val)){
                self->err.id = 26;        
                return (WpObject *)&self->err;
            }
        }
    }
    
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// function
    WasmBinFunction func;
    const uint8_t *code;
    for(i = 0; i < mod->function_count; i++){

        address = GetFunctionByIndex(mod->functionsec, i);
        if(!address){
            self->err.id = 27;        
            return (WpObject *)&self->err;
        }
        u32_data = DestructureFunctionIndex(address);
        if(u32_data > mod->type_count){
            self->err.id = 27;        
            return (WpObject *)&self->err;
        }
        address = GetTypeByIndex(mod->typesec, u32_data);
        if(!address){
            self->err.id = 27;        
            return (WpObject *)&self->err;
        }
        code = GetCodeByIndex(mod->codesec, i);
        if(!code){
            self->err.id = 28;        
            return (WpObject *)&self->err;
        }
        
        func = DestructureCode(code);
        if(i == 0){
            mod->funcs = WpStoreAllocFunction(&self->store, mod, address, func.locals, func.body);
            if(!mod->funcs){
                self->err.id = 29;        
                return (WpObject *)&self->err;
            }
        }
        else{
            if(!WpStoreAllocFunction(&self->store, mod, address, func.locals, func.body)){
                self->err.id = 29;        
                return (WpObject *)&self->err;
            }
        }
    }
    
    
    return (WpObject *)mod;
    
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
 *
WpObject *WpRuntimeInvocateProgram(WpRuntimeState *self, WpModuleInstance *m_instance)
{

    Name main_func = {5, "main"};

    // 2-If the module does not have a main function, then: Return

    for (uint32_t i = 0; i < m_instance->exports.lenght; i++)
    {
        if (strncmp(m_instance->exports.elements[i].name.name, main_func.name, 5) == 0)
        {

            // 3-If the main function is not valid, then: Fail
            if (m_instance->exports.elements[i].export_type != WP_EXTERNAL_TYPE_FUNC)
            {
                self->err.id = 32;
#if WASPC_CONFIG_DEV_FLAG == 1
                strcpy_s(self->err.file, 64, "runtime/runtime.c");
                strcpy_s(self->err.func, 32, "WpRuntimeInvocateProgram");
#endif
                return (WpObject *)&self->err;
            }

            // 4-Invoke the main function
            funcaddr main = m_instance->exports.elements[i].value.func;

            return WpFuncInstanceInvoke(self, main, NULL, 0);
        }
    }

    // 3-If the main function is not found, then: Return Error
    self->err.id = 32;
#if WASPC_CONFIG_DEV_FLAG == 1
    strcpy_s(self->err.file, 64, "runtime/runtime.c");
    strcpy_s(self->err.func, 32, "WpRuntimeInvocateProgram");
#endif
    return (WpObject *)&self->err;
}
*/
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
 *
WpObject *WpFuncInstanceInvoke(WpRuntimeState *self, funcaddr func, Value *args, uint32_t argc)
{

    self->err.id = 40;
#if WASPC_CONFIG_DEV_FLAG == 1
    strcpy_s(self->err.file, 64, "runtime/runtime.c");
    strcpy_s(self->err.func, 32, "WpFuncInstanceInvoke");
#endif
    return (WpObject *)&self->err;
    
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
*/
