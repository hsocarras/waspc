/**
 * @file function_instance.h
 * @author Hector E. Socarras (hsocarras1987@gmail.com)
 * @brief Header file for webassembly function instances related to spec's runtime chapter
 * @version 0.1
 * @date 2024-06-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef WASPC_WEBASSEMBLY_RUNTIME_FUNCTION_INSTANCE_H
#define WASPC_WEBASSEMBLY_RUNTIME_FUNCTION_INSTANCE_H

#ifdef __cplusplus
    extern "C" {
#endif

#include "webassembly/runtime/module_instance.h"

#include <stdint.h>

/**
 * @brief A function instance is the runtime representation of a function. It effectively is a closure of the original function
 * over the runtime module instance of its originating module. The module instance is used to resolve references to
 * other definitions during execution of the function.
 *
 * funcinst ::= {type functype, module moduleinst, code func}
 * | {type functype, hostcode hostfunc}
 * 
 */
typedef struct FunctionInstance{

    //bool is_host_function;

    //functype type
    
    ModuleInstance *module;

    //locals

    uint8_t *body;

} FunctionInstance;


#ifdef __cplusplus
    }
#endif

#endif