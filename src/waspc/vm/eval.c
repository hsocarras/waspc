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
    int32_t aux_i32;

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
                self->ip = DecodeLeb128Int32(self->ip, &aux_i32);
                printf("%i \n", aux_i32);
                break;
        }

        //updating watch dog
        current_t = clock();
    }

    WpResultAddError(&result, WP_DIAG_ID_DECODE_LEB128_FAIL, W_DIAG_MOD_LIST_DECODER);
    return result;

    #undef READ_BYTE

}