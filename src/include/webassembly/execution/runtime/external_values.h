/**
 * @file values.h
 * @author Hector E. Socarras (hsocarras1987@gmail.com)
 * @brief Header file for webassembly values related to spec's runtime chapter
 * @version 0.1
 * @date 2024-06-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef WASPC_WEBASSEMBLY_RUNTIME_EXTERNAL_VALUES_H
#define WASPC_WEBASSEMBLY_RUNTIME_EXTERNAL_VALUES_H

#ifdef __cplusplus
    extern "C" {
#endif

/**
 * @brief An external value is the runtime representation of an entity that can be imported or exported. It is an address
 * denoting either a function instance, table instance, memory instance, or global instances in the shared store.
 *  externval ::= func funcaddr
 *                  |table tableaddr
 *                  | mem memaddr
 *                  | global globaladdr

 * Conventions
 * The following auxiliary notation is defined for sequences of external values. It filters out entries of a specific kind
 * in an order-preserving fashion:
 *  • funcs(externval*) = [funcaddr | (func funcaddr) ∈ externval*]
 *  • tables(externval*) = [tableaddr | (table tableaddr) ∈ externval*]
 *  • mems(externval*) = [memaddr | (mem memaddr) ∈ externval*]
 *  • globals(externval*) = [globaladdr | (global globaladdr) ∈ externval*] 
*/



#ifdef __cplusplus
    }
#endif

#endif