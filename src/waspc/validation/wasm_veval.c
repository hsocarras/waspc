/**
 * @file wasm_veval.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-21
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#include "validation/wasm_validator.h"
#include "validation/wasm_decoder.h"
#include "webassembly/structure/module.h"
#include "webassembly/structure/types.h"
#include "webassembly/binary/module.h"
#include "webassembly/binary/instructions.h"
#include "utils/leb128.h"

#include <stdint.h>

/**
 * @brief Function to push a value type onto the validator's value stack.   
 * This function checks if there is enough space in the stack before pushing.
 * @param self Pointer to the validator state
 * @param val_type The value type to push onto the stack
 */
uint8_t WpValPushValType(WpValidatorState *self, ValType val_type) {
    if (self->stk_ptr - self->val_stack < self->val_stack_size) {
        *(self->stk_ptr++) = val_type;
        return 1;
    }
    return 0; // Stack overflow
}

/**
 * @brief Function to pop a value type from the validator's value stack.
 * This function checks if the stack is not empty before popping.
 * @param self Pointer to the validator state
 * @param val_type The expected value type to pop from the stack
 * @return ValType The value type popped from the stack, or UNKNOW if the stack is empty or mismatched.
 */
ValType WpValPopExpectedValType(WpValidatorState *self, ValType val_type) {
    // Check if the stack is not empty before popping
    if (self->stk_ptr > self->val_stack) {
        // Check if the top of the stack matches the expected value type
        if(*(self->stk_ptr - 1) == val_type) {
            return *(--self->stk_ptr);
        } else {
            // Handle type mismatch error
            // This could be an error code or a specific action
            return UNKNOW; // Indicating type mismatch
        }
    }
    return UNKNOW; // Stack underflow, return unknown type
}

ValType WpValPopValType(WpValidatorState *self) {
    // Check if the stack is not empty before popping
    if (self->stk_ptr > self->val_stack) {        
        return *(--self->stk_ptr);        
    }
    return UNKNOW; // Stack underflow, return unknown type
}

/**
 * @brief Function to push a control frame onto the validator's control stack.
 * This function checks if there is enough space in the control stack before pushing.   
 * @param self Pointer to the validator state
 * @param frame The control frame to push onto the stack
 * @return uint8_t 1 on success, 0 on failure (e.g.,    stack overflow)
 */
uint8_t WpValPushCtrlFrame(WpValidatorState *self, ValCtrlFrame frame) {
    // Check if there is space in the control stack
    ValCtrlFrame *top_frame;
    if (self->ctr_stack_idx < self->ctr_stack_size) {
        top_frame = &self->ctrl_stack[self->ctr_stack_idx++];       /// take current index and increment the control stack index after
        // Initialize the control frame
        //top_frame->op = frame.op;
        top_frame->start_types = frame.start_types;
        top_frame->end_types = frame.end_types;
        //top_frame->val_stack = frame.val_stack;
        top_frame->unreachable = frame.unreachable;
        return 1; // Success
    }
    return 0; // Stack overflow
}

/**
 * @brief Function to pop a control frame from the validator's control stack.
 * This function checks if the control stack is not empty before popping.
 * @param self Pointer to the validator state
 * @return ValCtrlFrame The popped control frame, or an empty frame if the stack is empty
 */
ValCtrlFrame WpValPopCtrlFrame(WpValidatorState *self) {
    // Check if the control stack is not empty before popping
    if (self->ctr_stack_idx > 0) {
        return self->ctrl_stack[--self->ctr_stack_idx];
    }
    // Handle underflow error, return an empty Ctrl structure or handle as needed
    ValCtrlFrame empty_ctrl = {NULL, NULL, 0, NULL, 0, 0};
    return empty_ctrl; // Indicating an empty control frame
}


/**
 * @brief Function to evaluate a WebAssembly opcode in the validator.
 * This function processes the opcode and updates the validator state accordingly.  
 * It checks if the opcode is valid and if the value stack has enough space for the operation.
 * @param self Pointer to the validator state
 * @param opcode The opcode to evaluate
 * @return uint32_t 0 on error, 1 on success
 */
uint32_t WpValEvalOpcode(WpValidatorState *self, OpCode opcode) {

    uint32_t dec_u32; // auxiliary variable for decoded values   
    ValCtrlFrame *frame = &self->ctrl_stack[self->ctr_stack_idx - 1]; // Get the current control frame 

    switch (opcode) {        
        case OPCODE_I32_CONST:
            // Push I32 type onto the stack
            if (!WpValPushValType(self, WAS_I32)) {
                return 0; // Stack overflow error
            }
            //move instruction pointer forward
            frame->ip = DecodeLeb128UInt32(frame->ip, &dec_u32);
            if (!frame->ip) {
                return 0; // Decoding error
            }
            break;
        case OPCODE_END:
            // Pop the stack, expecting an end of expression
            if (self->ctr_stack_idx == 0) {
                return 0; // Control stack underflow
            }
            /// set end_types pointer to where the returned values begin in the value stack
            frame->end_types = self->stk_ptr - frame->end_types_len;
            /// move return types to start types position
            for (uint32_t i = 0; i < frame->end_types_len; i++) {
                frame->start_types[i] = frame->end_types[i];
            }
            /// Removing start types from the value stack
            self->stk_ptr = frame->start_types; // Reset stack pointer to the start types
            // Pop the control frame
            WpValPopCtrlFrame(self);
            break;
        
        default:
            return 0; // Unsupported opcode
    }
    return 1; // Success
}
