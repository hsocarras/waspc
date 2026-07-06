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
#include "utils/leb128.h"
#include "utils/hash_table_host_func.h"
#include "webassembly/bin.h"

#include <assert.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>

/**
 * @brief Resolves an imported entity for a module during instantiation.
 * Imported modules and their exports are looked up in the runtime's module hash table.
 * The function checks for the existence of the imported module and export, do not verifies type compatibility,
 * and returns the resolved object or an error if resolution fails.
 * @param self Pointer to the runtime state.
 * @param import The import description.
 * @return Pointer to the resolved object or an error object.
 */
static WpObject *WpRuntimerResolveImport(WpRuntimeState *self, WasmBinImport import)
{   
    //Check if the import module name is valid
    if (import.module_name_len == 0 || import.module_name == NULL)
    {   
        self->err.id = __LINE__;    //TODO
        return (WpObject *)&self->err;
    }
    
    //Check if import name is buildin on the runtime. ///////////////////////////////
    if(strncmp((const char *)import.module_name, IEC_STD, import.module_name_len) == 0)
    {   
       
        //Check if the import name is in the buildin host functions hash table
        WpBuildinFunction *host_func = HashTableHostFuncGet(&self->host_funcs_std, (const char *)import.name, import.name_len);
        if (!host_func)
        {   
            self->err.id = __LINE__;    //TODO
            return (WpObject *)&self->err;
        }
        
        
        return (WpObject *)host_func;
    }

    /// Look for instantiate module in the hash table using import module name, if not found fail
    // Get module instance from hash table using import module name, if not found fail
    WpModuleState *import_mod = HashTableModulesGet(&self->modules, (const char *)import.module_name, import.module_name_len);
    
    if (!import_mod)
    {   
        self->err.id = 23;
        return (WpObject *)&self->err;
    }
    if (import_mod->instance == NULL)
    {   
        self->err.id = 23;
        return (WpObject *)&self->err;
    }

    WpModuleInstance *instance = import_mod->instance;
    // look for export that match the import description in the module instance exports, if not found or type mismatch fail
    for (uint32_t j = 0; j < instance->export_count; j++)
    {   
        
        if (instance->exports[j].name_len == import.name_len && strncmp((const char *)instance->exports[j].name, (const char *)import.name, import.name_len) == 0)
        {   
            WpExportInstance export = instance->exports[j];
            // check type match
            if (export.export_type != import.external_type)
            {
                self->err.id = 23;
                return (WpObject *)&self->err;
            }
            
            // Handle import according to its type
            switch (import.external_type)
            {
            case 0: // func                
                WpFunctionInstance *func_instance = (WpFunctionInstance *)export.address;                
                /// return function instace address to be assigned to module instance imports list
                return (WpObject *)func_instance;
            case 1: // table
                // TODO
                return (WpObject *)&self->err;
                break;
            case 2: // memory                
                WpMemoryInstance *memory_instance = (WpMemoryInstance *)export.address;
                if (IsMemoryTypeMatch(memory_instance->mem_type, import.external) == 0)
                {   
                    self->err.id = 23;
                    /// TODO add more info for error diagnostics.
                    return (WpObject *)&self->err;
                }                
                /// return memory instace address to be assigned to module instance imports list
                return (WpObject *)memory_instance;
                break;
            case 3: // global
                WpGlobalInstance *global_instance = (WpGlobalInstance *)export.address;
                // StackValType val_type = DestructureStackValType(import.external);
                // TODO asign global instance to module instance imports list
                return (WpObject *)&self->err;
                break;
            case 4: // tag
                // TODO check tag type match
                // TODO asign tag instance to module instance imports list
                return (WpObject *)&self->err;
                break;
            default:
                self->err.id = 27;
                return (WpObject *)&self->err;
            }
        }
    }
    self->err.id = 28;
    return (WpObject *)&self->err;
}

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

    // Init Memories
    self->data_memory = NULL;
    self->data_memory_size = 0;
    self->value_stack = NULL;
    self->value_stack_size = 0;
    self->call_stack = NULL;
    self->call_stack_size = 0;
    WpStoreInit(&self->store); //
    HashTableModulesInit(&self->modules); // Initialize hash table with zero capacity, will be set later
    //Init hash table for host functions
    self->host_funcs_std.entries = host_func_entries_std;
    self->host_funcs_std.capacity = 96;
    self->host_funcs_std.length = 2;
    // Interpreter
    WpInterpreterInit(&self->interpreter);
    self->interpreter.store = &self->store;
    self->interpreter.value_stack = self->value_stack;
    self->interpreter.value_stack_top = self->value_stack;
    self->interpreter.value_stack_end = self->value_stack + self->value_stack_size;

    // Init validator
    WpValidatorStateInit(&self->validator);
    self->validator.err = &self->err;
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

uint32_t WpRuntimeSetMemoryHashTable(WpRuntimeState *self, HtModuleEntry *mods, uint32_t entries)
{
    if (!mods || entries == 0)
    {
        self->err.id = 30;
        return 1;
    }
    self->modules.entries = mods;
    self->modules.capacity = entries;

    // Initialize entries
    for (int i = 0; i < entries; i++)
    {
        mods[i].key[0] = '\0';
        WpModuleStateInit(&mods[i].module);
    }

    self->modules.length = 0;

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

/**
 * @brief Creates a module state from a binary file and adds it to the runtime's module hash table.
 */
WpObject * WpRuntimeCreateModuleFromBinFile(WpRuntimeState *self, WpBinFile bin_file, const char *mod_name)
{   
    if (!bin_file.buf || bin_file.bufsize == 0)
    {
        self->err.id = 10;
#if WASPC_CONFIG_DEV_FLAG == 1
        self->err.module_id = 1;
#endif
        return (WpObject *)&(self->err);
    }

    if (!mod_name)
    {
        self->err.id = 12;
#if WASPC_CONFIG_DEV_FLAG == 1
        self->err.module_id = 1;
#endif
        return (WpObject *)&(self->err);
    }

    if (self->modules.entries == NULL || self->modules.capacity == 0)
    {
        self->err.id = 30;
        return (WpObject *)&(self->err);
    }

    WpModuleState mod_state;
    WpModuleStateInit(&mod_state);
    mod_state.buf = bin_file.buf;
    mod_state.bufsize = bin_file.bufsize;
    size_t mod_name_len = strlen(mod_name);
    
    // Add module to hash table
    WpModuleState *module = HashTableModulesSet(&self->modules, mod_name, mod_name_len, mod_state);
    if (!module)
    {
        self->err.id = 30;
        return (WpObject *)&(self->err);
    } 
    return (WpObject *)module;
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
        self->err.module_id = 1;
#endif
        return (WpObject *)&self->err;
    }

    // Check minimun module size for magic and version number ///////////////////////////////////
    if (mod->bufsize < 12)
    {
        self->err.id = 5;
        mod->status = WP_MODULE_STATUS_INVALID;
#if WASPC_CONFIG_DEV_FLAG == 1
        self->err.module_id = 1;
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
 * @return WpObject* Returns the instantiated module object on success,
 *                   or an error object on failure.
 */
WpObject *WpRuntimeInstanciateModule(WpRuntimeState *self, WpModuleState *mod)
{
    const uint8_t *address;
    uint32_t u32_data;
    size_t i;

    if (!mod)
    {
        self->err.id = 21;
#if WASPC_CONFIG_DEV_FLAG == 1
        self->err.module_id = 1;
#endif
        return (WpObject *)&self->err;
    }
    
    /// 1 If module is not valid, then://///////////////////////////////////////////////////////////////////////////////
    //  a. Fail
    if (mod->status != WP_MODULE_STATUS_VALIDATED)
    {
        WpObject *result = WpRuntimeValidateModule(self, mod);        
        if (!result)
        {
            return (WpObject *)&self->err;
        }
        if (result->wp_type == WP_OBJECT_ERROR)
        {
            return (WpObject *)&self->err;
        }
        if (result->wp_type == WP_OBJECT_MODULE_STATE)
        {
            if (((WpModuleState *)result)->status != WP_MODULE_STATUS_VALIDATED)
            {
                return (WpObject *)&self->err;
            }
        }
    }
    //Init module instance
    WpModuleInstance mod_instance;
    WpModuleInstanceInit(&mod_instance);

    // Allocating module instance and asign to module state, we need to asign it before resolve imports because of circular dependencies between modules
    WpModuleInstance *mod_instance_alloc = WpStoreAllocModule(&self->store, &mod_instance);    
    if (!mod_instance_alloc)
    {
        self->err.id = 21;
#if WASPC_CONFIG_DEV_FLAG == 1
        self->err.module_id = 1;
#endif
        return (WpObject *)&self->err;
    }
    ///////////////////////////////////////////////////////////////////////////////////////////////////////////
    // Types    
    if (mod->typesec.size > 0)
    {
        WpWasDefType def_type;
        uint32_t type_count;
        uint32_t aux;
        DecodeLeb128UInt32(mod->typesec.content, &type_count);
        mod_instance_alloc->type_count = type_count;

        // read binary section and create defined types for the module instance.
        for (i = 0; i < type_count; i++)
        {
            address = GetTypeByIndex(mod->typesec, i);
            if (!address)
            {
                self->err.id = 21;
                return (WpObject *)&self->err;
            }

            // Destructure TODO
            WpWasDefTypeInit(&def_type, WP_WAS_DEF_TYPE_FUNC_TYPE);
            def_type.binary = address;  //pointer to 0x60 byte in binary file where the defined type is defined, it can be used to compare with other defined type without decode them into WasmValueType.
            WasmBinFuncType func_type = DestructureFunctionType(address);

            
            def_type.def.func_type.param_len = func_type.param_len;
            def_type.def.func_type.ret_len = func_type.ret_len;
            def_type.def.func_type.param_types = func_type.param_types;
            def_type.def.func_type.ret_types = func_type.ret_types;

            if (i == 0)
            {
                mod_instance_alloc->types = WpStoreAllocDefType(&self->store, &def_type);
                if (!mod_instance_alloc->types)
                {
                    self->err.id = 21;
                    return (WpObject *)&self->err;
                }
            }
            else
            {
                if (!WpStoreAllocDefType(&self->store, &def_type))
                {
                    self->err.id = 21;
                    return (WpObject *)&self->err;
                }
            }
        }
    }
    //////8 ///////////////////////////////////////////////////////////////////////////////////////////////////
    // Import resolution    
    if (mod->importsec.size > 0)
    {   
        WasmBinImport import;
        // get imports from binary section and resolve them with the hash table, then asign the resolved imports to the module instance.
        //  If any import is not found or has a type mismatch, fail.
        DecodeLeb128UInt32(mod->importsec.content, &u32_data); // get the number of imports        
        for (i = 0; i < u32_data; i++)
        {
            address = GetImportByIndex(mod->importsec, i);            
            if (!address)
            {
                self->err.id = 22;
                return (WpObject *)&self->err;
            }
            import = DestructureImport(address);
            
            // Resolve import with hash table and check type match, if not found or type mismatch fail
            WpObject *resolved_import = WpRuntimerResolveImport(self, import);            
            if (!resolved_import)
            {
                return (WpObject *)&self->err;
            }
            if (resolved_import->wp_type == WP_OBJECT_ERROR)
            {
                return (WpObject *)&self->err;
            }

            switch (import.external_type)
            {
            case 0: // func
            {   
                if(resolved_import->wp_type != WP_OBJECT_FUNCTION_BUILDIN && resolved_import->wp_type != WP_OBJECT_FUNCTION_INSTANCE)
                {
                    self->err.id = 23;
                    return (WpObject *)&self->err;
                }

                // Create a function instance for the resolved import and asign it to module instance imports list, we need to create a new function instance because the same imported function can be used by multiple modules and each module needs its own instance of the imported function to store its own state during execution. For example, if two modules import the same function and they are called at the same time, they will have different call stacks and local variables, so they need different instances of the imported function to avoid conflicts.
                WpFunctionInstance func_instance;
                WpFunctionInstanceInit(&func_instance, WP_FUNC_IMPORT);                

                //Section for build in function ///////////////////////////////////////////////////////////////////////////////
                if(resolved_import->wp_type == WP_OBJECT_FUNCTION_BUILDIN)
                {                     
                    WpBuildinFunction *func_buildin = (WpBuildinFunction *)resolved_import;
                    uint32_t type_index;
                    DecodeLeb128UInt32(import.external, &type_index);
                    if(IsFuncTypeMatch(func_buildin->func_type->binary, mod_instance_alloc->types[type_index].binary) == 0)
                    {
                        self->err.id = __LINE__;
                        return (WpObject *)&self->err;
                    }

                    func_instance.func_kind = WP_FUNC_HOST;
                    func_instance.host_func = func_buildin->func_ptr;
                    func_instance.func_type = func_buildin->func_type;
                }

                if(resolved_import->wp_type == WP_OBJECT_FUNCTION_INSTANCE)
                {
                    //Check function type match
                    WpFunctionInstance *func_instance_imported = (WpFunctionInstance *)resolved_import;
                    uint32_t type_index;
                    DecodeLeb128UInt32(import.external, &type_index);
                    if(IsFuncTypeMatch(func_instance_imported->func_type->binary, mod_instance_alloc->types[type_index].binary) == 0)
                    {
                        self->err.id = 23;
                        return (WpObject *)&self->err;
                    }

                    // Assign the resolved import function instance to the new function instance
                    func_instance.func_kind = WP_FUNC_IMPORT;
                    func_instance.address = (WpFunctionInstance *)resolved_import;
                    func_instance.func_type = ((WpFunctionInstance *)resolved_import)->func_type;
                    func_instance.body = ((WpFunctionInstance *)resolved_import)->body;
                    func_instance.body_end = ((WpFunctionInstance *)resolved_import)->body_end;
                    func_instance.locals = ((WpFunctionInstance *)resolved_import)->locals; 
                }
                                
                

                // Allocation
                WpFunctionInstance *func_address = WpStoreAllocFunctionInstance(&self->store, &func_instance);
                if (!func_address)
                {
                    self->err.id = 23;
                    return (WpObject *)&self->err;
                }

                if (mod_instance_alloc->funcs == NULL)
                {
                    mod_instance_alloc->funcs = func_address;
                }

                mod_instance_alloc->function_count++;

                break;
            }
            case 1: // table
                // TODO check table type match
                // TODO asign table instance to module instance imports list
                return (WpObject *)&self->err;
                break;
            case 2: // memory
            {                
                if (resolved_import->wp_type != WP_OBJECT_MEMORY_INSTANCE)
                {
                    self->err.id = 23;
                    return (WpObject *)&self->err;
                }
                WpMemoryInstance mem_instance;
                WpMemoryInstanceInit(&mem_instance);
                mem_instance.imported = 1;
                mem_instance.address = ((WpMemoryInstance *)resolved_import);
                mem_instance.mem_type = ((WpMemoryInstance *)resolved_import)->mem_type;
                mem_instance.page_size_min = ((WpMemoryInstance *)resolved_import)->page_size_min;
                mem_instance.page_size_max = ((WpMemoryInstance *)resolved_import)->page_size_max;
                mem_instance.bytes = ((WpMemoryInstance *)resolved_import)->bytes;
                break;
            }
            case 3: // global
                // TODO check global type match
                // TODO asign global instance to module instance imports list
                return (WpObject *)&self->err;
                break;
            case 4: // tag
                // TODO check tag type match
                // TODO asign tag instance to module instance imports list
                return (WpObject *)&self->err;
                break;

            default:
                self->err.id = 25;
                return (WpObject *)&self->err;
            }
        }
    }
    //////16  ////////////////////////////////////////////////////////////////////////////////////////////////
    CallFrame f_init;
    f_init.module = NULL; // TODO: set module instance when module instance struct is defined
    f_init.locals = NULL;
    f_init.locals_count = 0;
    f_init.arity = 0;
    f_init.ip = NULL;
    f_init.blocks = NULL;
    /// 11, 12, 13 y 19 /////////////////////////////////////////////////////////////////////////////////////////////////////////

    /// Globals/////////////////////////////////////////////////////////////////////////////////////////////////////////
    if (mod->globalsec.size > 0)
    {   
        WpGlobalInstance global_instance;
        WpGlobalInstanceInit(&global_instance);
        WasmBinGlobal global;
        uint32_t global_count;
        DecodeLeb128UInt32(mod->globalsec.content, &global_count); // get the number of globals
        mod_instance_alloc->global_count = global_count;

        // read binary section and create global instances for the module instance.
        for (i = 0; i < mod_instance_alloc->global_count; i++)
        {
            // read binary section
            address = GetGlobalByIndex(mod->globalsec, i);
            if (!address)
            {
                self->err.id = 24;
                return (WpObject *)&self->err;
            }

            /// destructuring global
            global = DestructureGlobal(address);
            StackValType val_type = DestructureStackValType(global.type);
            if (val_type == WAS_EX_VAL_TYPE_NULL)
            {
                self->err.id = 24;
                return (WpObject *)&self->err;
            }
            f_init.ip = global.init_expr;                      // set the instruction pointer of the initial frame to the global init expression
            f_init.arity = 1;                                  // set the arity of the initial frame to 1, because global init expression must return a value
            f_init.locals = self->interpreter.value_stack_top; // set the locals of the initial frame to the current top of the value stack, because global init expression can use the value stack to store intermediate values during evaluation

            StackValue val = WpInterpreterEvalExpr(&self->interpreter, &f_init); // evaluate the global init expression and get the value
            if (val.type == WAS_EX_VAL_TYPE_NULL)
            {
                self->err.id = 26;
                return (WpObject *)&self->err;
            }
            global_instance.mut = global.mut;
            global_instance.type = val_type;
            global_instance.val = val;

            //Remember that first global can be an imported global, so we need to check if the global list is NULL or not.
            if (mod_instance_alloc->globals == NULL)
            {
                // if first global asign address to module instances list
                mod_instance_alloc->globals = WpStoreAllocGlobal(&self->store, &global_instance);
                if (!mod_instance_alloc->globals)
                {
                    self->err.id = 25;
                    return (WpObject *)&self->err;
                }
            }
            else
            {
                if (!WpStoreAllocGlobal(&self->store, &global_instance))
                {
                    self->err.id = 26;
                    return (WpObject *)&self->err;
                }
            }
        }
    }
    // Memory///////////////////////////////////////////////////////////////////////////////////////////////////////////    
    if (mod->memsec.size > 0)
    {   
        WpMemoryInstance mem_instance;
        WpMemoryInstanceInit(&mem_instance);
        WasmBinMemory memory;
        uint32_t mem_count;
        DecodeLeb128UInt32(mod->memsec.content, &mem_count);
        mod_instance_alloc->mem_count = mem_count;

        // read binary section and create memory instances for the module instance.
        for (i = 0; i < mem_count; i++)
        {
            address = GetMemByIndex(mod->memsec, i);
            mem_instance.mem_type = address;
            if (!address)
            {
                self->err.id = 27;
                return (WpObject *)&self->err;
            }

            memory = DestructureMemory(address);
            
            //Remember that first memory can be an imported memory, so we need to check if the memory list is NULL or not.
            if (mod_instance_alloc->mems == NULL)
            {
                mod_instance_alloc->mems = WpStoreAllocMemory(&self->store, &mem_instance, self->data_memory);
                if (!mod_instance_alloc->mems)
                {
                    self->err.id = 28;
                    return (WpObject *)&self->err;
                }
            }
            else
            {
                if (!WpStoreAllocMemory(&self->store, &mem_instance, self->data_memory))
                {
                    self->err.id = 28;
                    return (WpObject *)&self->err;
                }
            }
        }
    }
    //////////////////////////////////////////////////////////////////////////////////////////////////////////////////

    /// function//////////////////////////////////////////////////////////////////////////////////////////////////////
    if (mod->functionsec.size > 0)
    {  
        WpFunctionInstance func_instance;
        WpFunctionInstanceInit(&func_instance, WP_FUNC_NORMAL);
        WasmBinFunction func;
        const uint8_t *code;
        uint32_t func_count;
        DecodeLeb128UInt32(mod->functionsec.content, &func_count);
        mod_instance_alloc->def_function_count = func_count;

        // read binary section and create function instances for the module instance.
        for (i = 0; i < func_count; i++)
        {

            address = GetFunctionByIndex(mod->functionsec, i);
            if (!address)
            {
                self->err.id = 27;
                return (WpObject *)&self->err;
            }

            //Get function type index and check if it's valid from function section.
            u32_data = DestructureFunctionIndex(address);
            if (u32_data > mod_instance_alloc->type_count || mod_instance_alloc->types == NULL)
            {
                self->err.id = 27;
                return (WpObject *)&self->err;
            }

            func_instance.module = mod_instance_alloc;
            func_instance.func_type = &mod_instance_alloc->types[u32_data];

            code = GetCodeByIndex(mod->codesec, i);
            if (!code)
            {
                self->err.id = 28;
                return (WpObject *)&self->err;
            }

            func = DestructureCode(code);
            func_instance.locals = func.locals;
            func_instance.body = func.body;
            func_instance.body_end = func.end;

            //Check for function instance allocation, if it's the first function instance, asign the address to module instance function list, otherwise just allocate the function instance.
            //Remember that first function can be an imported function, so we need to check if the function list is NULL or not.
            if (mod_instance_alloc->funcs == NULL)
            {   
                mod_instance_alloc->funcs = WpStoreAllocFunctionInstance(&self->store, &func_instance);
                if (!mod_instance_alloc->funcs)
                {
                    self->err.id = 29;
                    return (WpObject *)&self->err;
                }
            }
            else
            {
                if (!WpStoreAllocFunctionInstance(&self->store, &func_instance))
                {
                    self->err.id = 29;
                    return (WpObject *)&self->err;
                }
            }
            mod_instance_alloc->function_count++;
        }
        
    }
    /// Export ///////////////////////////////////////////////////////////////////////////////////////////////////////
    if (mod->exportsec.size > 0)
    {   
        WpExportInstance export_instance;
        WpExportInstanceInit(&export_instance);
        WasmBinExport export;
        uint32_t export_count;
        DecodeLeb128UInt32(mod->exportsec.content, &export_count);
        mod_instance_alloc->export_count = export_count;
        
        for (i = 0; i < mod_instance_alloc->export_count; i++)
        {
            address = GetExportByIndex(mod->exportsec, i);
            if (!address)
            {
                self->err.id = 27;
                return (WpObject *)&self->err;
            }

            export = DestructureExport(address);
            export_instance.name_len = export.name_len;
            export_instance.name = export.name;
            export_instance.export_type = export.index_type;                        
            switch (export.index_type)
            {
            case 0: // func
                if(mod_instance_alloc->funcs == NULL || mod_instance_alloc->funcs[export.external_index].wp_type != WP_OBJECT_FUNCTION_INSTANCE){
                    self->err.id = 27;
                    return (WpObject *)&self->err;
                }
                export_instance.address = (WpObject *)&mod_instance_alloc->funcs[export.external_index];
                break;
            case 1: // table TODO
                /*if(mod_instance_alloc->tables == NULL || mod_instance_alloc->tables[export.external_index].wp_type != WP_OBJECT_TABLE_INSTANCE){
                    self->err.id = 27;
                    return (WpObject *)&self->err;
                }
                export_instance.address = &mod_instance_alloc->tables[export.external_index];*/
                break;
            case 2: // memory
                if(mod_instance_alloc->mems == NULL || mod_instance_alloc->mems[export.external_index].wp_type != WP_OBJECT_MEMORY_INSTANCE){
                    self->err.id = 27;
                    return (WpObject *)&self->err;
                }
                export_instance.address = (WpObject *)&mod_instance_alloc->mems[export.external_index];
                break;
            case 3: // global
                if(mod_instance_alloc->globals == NULL || mod_instance_alloc->globals[export.external_index].wp_type != WP_OBJECT_GLOBAL_INSTANCE){
                    self->err.id = 27;
                    return (WpObject *)&self->err;
                }
                export_instance.address = (WpObject *)&mod_instance_alloc->globals[export.external_index];
                break;
            case 4: // tag TODO                
                break;
            default:
                self->err.id = 28;
                return (WpObject *)&self->err;
            }

            if (i == 0)
            {
                mod_instance_alloc->exports = WpStoreAllocExport(&self->store, &export_instance);
                if (!mod_instance_alloc->exports)
                {
                    self->err.id = 29;
                    return (WpObject *)&self->err;
                }
            }
            else
            {
                if (!WpStoreAllocExport(&self->store, &export_instance))
                {
                    self->err.id = 29;
                    return (WpObject *)&self->err;
                }
            }
        }
    }
    ////////////////////////////////////////////////////////////////////////////////////////////////////////////////    
    mod->instance = mod_instance_alloc;
    mod->status = WP_MODULE_STATUS_INSTANTIATED;  
    return (WpObject *)mod_instance_alloc;
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

WpObject *WpRuntimeInvokeFunction(WpRuntimeState *self, WpModuleInstance *mod, char *func_name, StackValue *args, uint32_t argc)
{
    // look for export with func_name    
    for (uint32_t i = 0; i < mod->export_count; i++)
    {
        WpExportInstance export = mod->exports[i];
        if (export.export_type == 0x00)
        { // if export is a function
            // TODO match string len also
            WpFunctionInstance *func = (WpFunctionInstance *)export.address; // TODO check if export address is correct and if it is a function instance
            if (strncmp((const char *)export.name, func_name, export.name_len) == 0)
            {   
                ///////////////////////////////////////////////////////////////////////////////////////////
                /// TODO Step 2 to 5
                /// Step 6 and 7
                // TODO check call_stack overflow
                // CallFrame * frame = &self->interpreter.call_stack[self->interpreter.call_stack_count++]; //get top and add frame count
                // frame->arity = func->ret_len;
                /// Step 8
                for (uint32_t i = 0; i < argc; i++)
                {
                    PushValue(&self->interpreter, args[i]);
                }
                uint32_t error_code = InvokeFunctionFast(&self->interpreter, func);
                if (error_code != 0)
                {   
                    self->err.id = error_code;
                    return (WpObject *)&self->err;
                }
                // STEP 11. Pop the values val′𝑘 from the stack.
                // self->interpreter.value_stack_top -= func->ret_len;
                // Step 12. Pop the frame from the stack.
                // self->interpreter.call_stack_count--;
                // STEP 13. Return the values val′𝑘 as the result of the function invocation.

                self->result.vals = self->interpreter.value_stack_top - func->func_type->def.func_type.ret_len; // TODO check if value_stack_top is correct after function invocation and if it points to the return values
                self->result.len = func->func_type->def.func_type.ret_len;
                return (WpObject *)&self->result;
            }
        }
    }

    self->err.id = 36; // not found TODO better error handling.
    return (WpObject *)&self->err;
}
