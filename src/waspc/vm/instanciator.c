/**
 * @file instanciator.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-11-06
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "runtime/runtime.h"
#include "runtime/store.h"
#include "runtime/instances.h"
#include "vm/vm.h"
#include "decoder/wasm_loader.h"
#include "decoder/wasm_decoder.h"
#include "object/result.h"
#include "object/bin_module.h"
#include "object/decoded_module.h"
#include "memory/memory.h"
#include "webassembly/execution/runtime/instances.h"




static VectorGlobalInst InstantiateGlobal(VM *vm, WpDecodedModule *source, ModuleInst *target){

    Val init_val;
    GlobalInst instance;
    VectorGlobalInst instances;
    VectorGlobalInstInit(&instances, source->global_len + 2*18);        //2 more max (v128) global instances for spare
    
    if(!instances.global){
        //TODO
    }

    //if globals instances are allocated, allocated module instance global address
    target->globaladdrs = ALLOCATE(GlobalAddr, instances.len);
    if(!target->globaladdrs){
        //TODO
    }
    target->globaladdrs_len = instances.len;

    for(int i = 0; i < source->global_len; i++){
           
        instance.mut = source->globals[i].mut;
        instance.val_type = source->globals[i].type;
        if(source->globals[i].expr_len > 0){
            init_val = VmExecuteExpresion(vm,source->globals[i].expr);            
        }
        else{
            init_val.u32 = 0; //TODO default            
        }
        instance.value = init_val;        
        target->globaladdrs[i] = VectorGlobalInstWrite(&instances, &instance);        
    }
    
    return instances;

}

WpResult InstantiateModule(VM *self, const uint8_t * const buf, const uint32_t size){

    WpResult result;
    WpResultInit(&result);
     
    WpBinModule bin_mod;
    result = LoadWasmBuffer(buf, size, &bin_mod);

    WpDecodedModule d_mod;

    if(result.result_type == WP_OBJECT_RESULT_TYPE_ERROR ){
        return result;
    }

    result = DecodeTypeSection(&bin_mod.typesec, &d_mod);
    if(result.result_type == WP_OBJECT_RESULT_TYPE_ERROR ){
        return result;
    }

    ModuleInst mod;
    mod.types_len = d_mod.type_len;
    mod.types = d_mod.types;
    
    
    result = DecodeFunctionSection(&bin_mod.functionsec, &d_mod);
    if(result.result_type == WP_OBJECT_RESULT_TYPE_ERROR ){
        return result;
    } 
    
    result = DecodeGlobalSection(&bin_mod.globalsec, &d_mod);
    if(result.result_type == WP_OBJECT_RESULT_TYPE_ERROR){
            return result;
    }
    self->store->first.global_inst = InstantiateGlobal(self, &d_mod, &mod);

    result = DecodeCodeSection(&bin_mod.codesec, &d_mod);

    mod.main.body = d_mod.funcs[0].body;
    self->mod = mod;

    return result;

}