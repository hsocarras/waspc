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
#include "webassembly/bin.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>


/**
 * @brief Init function for runtime state object
 *
 * @param self
 */
void WpRuntimeInit(WpRuntimeState *self)
{

    // Init error object
    WpErrorInit(&self->err);
    // Init result object
    WpResultInit(&self->result);

    //Init Memories
    self->data_memory = NULL;
    self->data_memory_size = 0;
    self->value_stack = NULL;
    self->value_stack_size = 0;
    self->call_stack = NULL;
    self->call_stack_size = 0; 
    WpStoreInit(&self->store); //

    // Interpreter
    WpInterpreterInit(&self->interpreter);
    self->interpreter.store = &self->store;
    self->interpreter.value_stack = self->value_stack;
    self->interpreter.value_stack_top = self->value_stack;
    self->interpreter.value_stack_end = self->value_stack + self->value_stack_size;
    

    // Init validator
    WpValidatorStateInit(&self->validator);
}

uint32_t WpRuntimeSetMemoryStore(WpRuntimeState *self, uint8_t *mem, uint32_t mem_size)
{
    if (!mem || mem_size == 0)
    {
        self->err.id = 31;
        return 1;
    }
    self->store.buffer = mem;
    self->store.buffer_size = mem_size;
    self->store.buffer_free = mem;

    self->interpreter.store = &self->store;
    return 0;
}

uint32_t WpRuntimeSetMemoryData(WpRuntimeState *self, uint8_t *data, uint32_t data_size)
{
    if (!data || data_size == 0)
    {
        self->err.id = 32;
        return 1;
    }
    self->data_memory = data;
    self->data_memory_size = data_size;
    return 0;

    self->interpreter.block_stack = data;
    self->interpreter.block_stack_top = data;
    self->interpreter.block_stack_end = data + data_size;
}

uint32_t WpRuntimeSetMemoryValueStack(WpRuntimeState *self, StackValue *stack, uint32_t stack_size)
{
    if (!stack || stack_size == 0)
    {
        self->err.id = 33;
        return 1;
    }
    self->value_stack = stack;
    self->value_stack_size = stack_size;

    self->interpreter.value_stack = stack;
    self->interpreter.value_stack_top = stack;  
    self->interpreter.value_stack_end = stack + stack_size;

    self->validator.value_stack = stack;
    self->validator.value_stack_top = stack;
    self->validator.value_stack_end = stack + stack_size;

    return 0;
}

uint32_t WpRuntimeSetMemoryCallStack(WpRuntimeState *self, CallFrame *stack, uint32_t stack_size)
{
    if (!stack || stack_size == 0)
    {
        self->err.id = 34;
        return 1;
    }
    self->call_stack = stack;
    self->call_stack_size = stack_size;   
    self->interpreter.call_stack = stack;
    self->interpreter.call_stack_size = stack_size; 
    
    return 0;
}

WpObject *WpRuntimeCreateModuleFromBinFile(WpRuntimeState *self, WpModuleState *mod_state, WpBinFile bin_file)
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
    
    return WpValidatorValidateModule(&self->validator, mod);
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
    printf("Instantiating module...\n");
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
    //////16  ////////////////////////////////////////////////////////////////////////////////////////////////
    CallFrame f_init;
    f_init.module = NULL; //TODO: set module instance when module instance struct is defined
    f_init.locals = NULL;
    f_init.locals_count = 0;
    f_init.arity = 0;
    f_init.ip = NULL;
    f_init.blocks = NULL;
    /// 11, 12, 13 y 19 /////////////////////////////////////////////////////////////////////////////////////////////////////////
    /// Globals
    {   printf("Instantiating globals...\n");
        WasmBinGlobal global;
        for(i = 0; i < mod->global_count; i++){
            //read binary section
            address = GetGlobalByIndex(mod->globalsec, i);
            if(!address){
                self->err.id = 24;        
                return (WpObject *)&self->err;
            }
            /// destructuring global
            printf("Destructuring global %d...\n", i);
            global = DestructureGlobal(address);
            StackValType val_type = DestructureStackValType(global.type);
            if(val_type == WAS_EX_VAL_TYPE_NULL){
                self->err.id = 24;        
                return (WpObject *)&self->err;
            }
            f_init.ip = global.init_expr; //set the instruction pointer of the initial frame to the global init expression
            f_init.arity = 1; //set the arity of the initial frame to 1, because global init expression must return a value
            f_init.locals = self->interpreter.value_stack_top; //set the locals of the initial frame to the current top of the value stack, because global init expression can use the value stack to store intermediate values during evaluation
            
            StackValue val = WpInterpreterEvalExpr(&self->interpreter, &f_init); //evaluate the global init expression and get the value
            if(val.type == WAS_EX_VAL_TYPE_NULL){
                self->err.id = 26;        
                return (WpObject *)&self->err;
            }
            if(i == 0){
                //if first global asign addres to module instances list
                mod->globals = WpStoreAllocGlobal(&self->store, global.mut, val_type, val);
                if(!mod->globals){
                    self->err.id = 25;        
                    return (WpObject *)&self->err;
                }
            }
            else{
                if(!WpStoreAllocGlobal(&self->store, global.mut, val_type, val)){
                    self->err.id = 26;        
                    return (WpObject *)&self->err;
                }
            }
        }
    }
    //Memory///////////////////////////////////////////////////////////////////////////////////////////////////////////
    {   printf("Instantiating memories...\n");
        WasmBinMemory memory;
        for(i = 0; i < mod->memory_count; i++){
            address = GetMemByIndex(mod->memsec, i);
            if(!address){
                self->err.id = 27;        
                return (WpObject *)&self->err;
            }
            
            memory = DestructureMemory(address);

            if(i == 0){
                mod->mems = WpStoreAllocMemory(&self->store, memory, self->data_memory);
                if(!mod->mems){
                    self->err.id = 28;        
                    return (WpObject *)&self->err;
                }
            }
            else{
                if(!WpStoreAllocMemory(&self->store, memory, self->data_memory)){
                    self->err.id = 28;        
                    return (WpObject *)&self->err;
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////    
    /// function//////////////////////////////////////////////////////////////////////////////////////////////////////
    WasmBinFunction func;
    WasmBinFuncType type;
    printf("function count: %d\n", mod->function_count);
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
            mod->funcs = WpStoreAllocFunction(&self->store, mod, type, func);
            if(!mod->funcs){
                self->err.id = 29;        
                return (WpObject *)&self->err;
            }
        }
        else{
            if(!WpStoreAllocFunction(&self->store, mod, type, func)){
                self->err.id = 29;        
                return (WpObject *)&self->err;
            }
        }
    }
    /// Export ///////////////////////////////////////////////////////////////////////////////////////////////////////
    {
        WasmBinExport export;
        for(i = 0; i < mod->export_count; i++){
            address = GetExportByIndex(mod->exportsec, i);
            if(!address){
                self->err.id = 27;        
                return (WpObject *)&self->err;
            }
            export = DestructureExport(address);    
            if(i == 0){
                mod->exports = WpStoreAllocExport(&self->store, export, mod);
                if(!mod->exports){
                    self->err.id = 29;        
                    return (WpObject *)&self->err;
                }
            }
            else{
                if(!WpStoreAllocExport(&self->store, export, mod)){
                    self->err.id = 29;        
                    return (WpObject *)&self->err;
                }
            }
        }
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////////////
    
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
 *
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
    CallFrame * frame = &self->interpreter.call_stack[self->interpreter.call_stack_count++]; //get top and add frame count
    frame->arity = 1;  //TODO arity come from previous step
        
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
}*/

WpObject *WpRuntimeInvokeFunction(WpRuntimeState *self, WpModuleState *mod, char *func_name, StackValue *args, uint32_t argc)
{ 
    //look for export with func_name
    for(uint32_t i = 0; i < mod->export_count; i++){
        WpExportInstance export = mod->exports[i];
        if(export.export_type == 0x00){ //if export is a function
            //TODO match string len also
            WpFunctionInstance *func = (WpFunctionInstance *)export.address; //TODO check if export address is correct and if it is a function instance
            if(strncmp((const char *)export.name, func_name, export.name_len) == 0){
                ///////////////////////////////////////////////////////////////////////////////////////////
                /// TODO Step 2 to 5 
                /// Step 6 and 7
                //TODO check call_stack overflow
                printf("Invoking function: %s\n", func_name);
                //CallFrame * frame = &self->interpreter.call_stack[self->interpreter.call_stack_count++]; //get top and add frame count                
                //frame->arity = func->ret_len;  
                /// Step 8
                for(uint32_t i = 0; i < argc; i++){
                    PushValue(&self->interpreter, args[i]);
                }
                uint32_t error_code = InvokeFunctionFast(&self->interpreter, func);
                if(error_code != 0){
                    self->err.id = error_code;
                    return (WpObject *)&self->err;
                }
                //STEP 11. Pop the values val′𝑘 from the stack.
                //self->interpreter.value_stack_top -= func->ret_len; 
                //Step 12. Pop the frame from the stack.
                //self->interpreter.call_stack_count--;
                //STEP 13. Return the values val′𝑘 as the result of the function invocation.
                
                self->result.vals = self->interpreter.value_stack_top-func->ret_len; //TODO check if value_stack_top is correct after function invocation and if it points to the return values
                self->result.len = func->ret_len;
                return (WpObject *)&self->result;          
            }
        }
    }

    self->err.id = 36; //not found TODO better error handling.
    return (WpObject *)&self->err;
}
