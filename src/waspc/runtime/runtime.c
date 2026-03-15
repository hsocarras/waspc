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
    WpInterpreterInit(&self->interpreter);
    self->interpreter.store = &self->store;

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
        StackValue val = WpInterpreterEvalExpr(&self->interpreter, global.init_expr);
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
    WasmBinFuncType type;

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
        type = DestructureFunctionType(address);

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
            mod->funcs = WpStoreAllocFunction(&self->store, mod, type, func.locals, func.body);
            if(!mod->funcs){
                self->err.id = 29;        
                return (WpObject *)&self->err;
            }
        }
        else{
            if(!WpStoreAllocFunction(&self->store, mod, type, func.locals, func.body)){
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
 * @param func Index to the function instance to invoke.
 * @param args Pointer to an array of argument values to pass to the function (parameters).
 * @param argc Number of argument values in the args array.
 * @return WpObject* Returns the result of the function invocation on success,
 *                   or an error object on failure.
 */
WpObject *WpFuncRuntimeInvoke(WpRuntimeState *self, uint32_t func_address, StackValue *args, uint32_t argc)
{
    /// Step 1//////////////////////////////////////////////////////////////////////////////////
    if(!self->store.funcs){
        self->err.id = 133;
        return (WpObject *)&self->err;
    }

    if(func_address >= self->store.func_count){
        self->err.id = 34;
        return (WpObject *)&self->err;
    }

    WpFunctionInstance *func = WpStoreGetFunctionByIndex(&self->store, func_address);
    if(!func){
        self->err.id = 35;
        return (WpObject *)&self->err;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////
    /// TODO Step 2 to 5 

    /// Step 6 and 7
    //TODO check call_stack overflow
    CtrlFrame * frame = &self->interpreter.ctrl_satck[self->interpreter.ctrl_count++]; //get top and add frame count
    frame->type = WP_INTERPRETER_CTRL_CALL_FRAME;
    frame->ctrl.call_frame.arity = 1;  //TODO arity come from previous step
        
    /// Step 8
    for(uint32_t i = 0; i < argc; i++){
        PushValue(&self->interpreter, args[i]);
    }

    /// Step 9
    StackValue func_ref;
    func_ref.type = WAS_VAL_REF_FUNC;
    func_ref.value.i32 = func_address;
    PushValue(&self->interpreter, func_ref);
    uint8_t error_code = WpInterpreterExecuteCallRefFunc(&self->interpreter, NULL);

    self->err.id = error_code;
    StackValue result = PopValue(&self->interpreter);
    
    self->err.code = result.value.i32;

    return (WpObject *)&self->err;
}

