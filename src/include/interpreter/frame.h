/**
 * @file module.h
 * @author Hector E. Socarras (hsocarras1987@gmail.com)
 * @brief Header file for webassembly virtual machine
 * @version 0.1
 * @date 2024-06-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef WASPC_INTERPRETER_FRAME_H
#define WASPC_INTERPRETER_FRAME_H

#ifdef __cplusplus
    extern "C" {
#endif

#include <stdint.h>

typedef struct WpInterpFrame {
    /* The frame of the caller that are calling the current function. */
    //struct WASMInterpFrame *prev_frame;

    /*module instance*/

    /*locals*/
    /* The current WASM function. */
    //struct WASMFunctionInstance *function;

    /* Instruction pointer of the bytecode array.  */
    uint8 *ip;
    
    /* Operand stack top pointer of the current frame. The bottom of
       the stack is the next cell after the last local variable. */
    uint32 *sp_bottom;
    uint32 *sp_boundary;
    uint32 *sp;

    WASMBranchBlock *csp_bottom;
    WASMBranchBlock *csp_boundary;
    WASMBranchBlock *csp;

    /**
     * Frame data, the layout is:
     *  lp: parameters and local variables
     *  sp_bottom to sp_boundary: wasm operand stack
     *  csp_bottom to csp_boundary: wasm label stack
     *  frame ref flags: only available for GC
     *    whether each cell in local and stack area is a GC obj
     *  jit spill cache: only available for fast jit
     */
    uint32 lp[1];
#endif /* end of WASM_ENABLE_FAST_INTERP != 0 */
} WASMInterpFrame;




#ifdef __cplusplus
    }
#endif

#endif
