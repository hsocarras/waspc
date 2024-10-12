/**
 * @file context.h
 * @author Hector E. Socarras (hsocarras1987@gmail.com)
 * @brief Header file for webassembly virtual machine
 * @version 0.1
 * @date 2024-06-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef WASPC_WEBASSEMBLY_VALIDATION_CONTEXT_H
#define WASPC_WEBASSEMBLY_VALIDATION_CONTEXT_H

#ifdef __cplusplus
    extern "C" {
#endif

#include "webassembly/structure/types.h"
#include "webassembly/structure/modules.h"

#include <stdint.h>



/**
 * @brief Validity of an individual definition is specified relative to a context, which collects relevant information about the
 * surrounding module and the definitions in scope:
 *
 * 𝐶 ::= { types functype*,
 *  funcs functype*,
 *  tables tabletype*,
 *  mems memtype*,
 *  globals globaltype*,
 *  elems reftype*,
 *  datas ok*,
 *  locals valtype*,
 *  labels resulttype*,
 *  return resulttype?,
 *  refs funcidx* }
 */
typedef struct WasContext {
    // type  
    uint32_t type_len;    
    FuncType *types;

    // funcs
    uint32_t funcs_len;    
    FuncType *funcs;

    //Tables
    uint32_t table_len;
    Table *tables;

    //Mems
    uint32_t mem_len;
    Mem *mems;

    //Global
    uint32_t global_len;
    Global *globals;

    //elems
    uint32_t element_len;
    ElementType elem;

    //data

    //locals

    //lables

    //returns

    //ref

} WasContext;



#ifdef __cplusplus
    }
#endif

#endif