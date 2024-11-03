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
#include "utils/leb128.h"

#include <stdio.h>
#include <stdint.h>
#include <time.h>

WpResult VmEvalFrame (VM *self){

    uint8_t instruction;
    VmValue c1;

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
            case OPCODE_RETURN:
                return result;
            case OPCODE_I32_CONST:
                c1.type = VM_NUM_I32;
                self->ip = DecodeLeb128Int32(self->ip, &c1.val.s32);
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