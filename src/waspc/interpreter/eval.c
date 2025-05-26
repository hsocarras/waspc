/**
 * @file eval.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-10-30
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "vm/vm.h"
#include "object/result.h"
#include "webassembly/binary/instructions.h"
#include "webassembly/execution/runtime/addresses.h"
#include "utils/leb128.h"

#include <stdio.h>
#include <stdint.h>
#include <time.h>


WpResult VmEvalFrame (VM *self){

    uint8_t instruction;
    VmValue c1;
    uint32_t aux_u32;

    WpResult result;      //wasp result object
    WpResultInit(&result);

    clock_t start_t, current_t;
    uint64_t watchdog = 2*CLOCKS_PER_SEC;    
    
    start_t = clock();
    current_t = clock();

    #define READ_BYTE() (*self->ip++)

    while(current_t - start_t < watchdog){
        instruction = READ_BYTE();

        printf("instruction %i\n", instruction);

        switch(instruction){
            case OPCODE_END:
                c1 = VmPopValue(self);
                //TODO better type 
                result.value.was = c1.as;
                return result;
            case OPCODE_RETURN:
                return result;
            case OPCODE_GLOBAL_GET:
                //0x23 𝑥:globalidx ⇒ global.get 𝑥
                self->ip = DecodeLeb128UInt32(self->ip, &aux_u32);       //get globalidx                
                //Step 3 Let 𝑎 be the global address 𝐹.module.globaladdrs[𝑥].
                GlobalAddr a =self->mod.globaladdrs[aux_u32];                
                //Step 4 Assert: due to validation, 𝑆.globals[𝑎] exists.
                if(a >= self->store->first.global_inst.global && a < self->store->first.global_inst.top){
                    //Step 5 Let glob be the global instance 𝑆.globals[𝑎].
                    GlobalInst g = VectorGlobalInstRead(a);                    
                    //Step 6 Let val be the value glob.value.
                    c1.type = g.val_type;
                    c1.as = g.value;
                    //Step 7 Push the value val to the stack
                    result = VmPushValue(self, c1);
                    if(result.result_type == WP_OBJECT_RESULT_TYPE_ERROR){
                        return result;
                    }
                    break;
                }
                else{
                    //trap
                }
                break;
            case OPCODE_I32_CONST:
                c1.type = WAS_I32;
                self->ip = DecodeLeb128Int32(self->ip, &c1.as.s32);    //5.2.2
                if(!self->ip){
                    WpResultAddError(&result, WP_DIAG_ID_ASSERT_EVAL_I32_CONST, W_DIAG_MOD_LIST_VM);
                    return result;
                }
                result = VmPushValue(self, c1);
                if(result.result_type == WP_OBJECT_RESULT_TYPE_ERROR){
                    return result;
                }
                break;
        }

        //updating watch dog
        current_t = clock();
    }

    WpResultAddError(&result, WP_DIAG_ID_ASSERT_EVAL_WATCHDOG, W_DIAG_MOD_LIST_VM);
    return result;

    #undef READ_BYTE

}