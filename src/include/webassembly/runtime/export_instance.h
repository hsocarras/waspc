/**
 * @file export_instance.h
 * @author Hector E. Socarras (hsocarras1987@gmail.com)
 * @brief Header file for webassembly export instances related to spec's runtime chapter
 * @version 0.1
 * @date 2024-06-26
 * 
 * @copyright Copyright (c) 2024
 * 
 */

#ifndef WASPC_WEBASSEMBLY_RUNTIME_EXPORT_INSTANCE_H
#define WASPC_WEBASSEMBLY_RUNTIME_EXPORT_INSTANCE_H

#ifdef __cplusplus
    extern "C" {
#endif

#include <stdint.h>

/**
 * @brief Function instances, table instances, memory instances, and global instances, element instances, and data instances
 * in the store are referenced with abstract addresses.
 * addr ::= 0 | 1 | 2 | . . .
 */
typedef uint16_t addr;

//An external value is the runtime representation of an entity that can be imported or exported. It is an address
//denoting either a function instance, table instance, memory instance, or global instances in the shared store.

//externval ::= func funcaddr
//              | table tableaddr
//              | mem memaddr
//              | global globaladdr

/**
 * @brief Enum with the diferent external values's type definition acording webassembly spec.
 * 
 */
typedef enum {
    FUNC,
    TABLE,
    MEM,
    GLOBAL,
} ExternValType;

/**
 * @brief 
 * 
 */
typedef struct {
    ExternValType type;
    addr address;
}ExternVal;


/**
 * @brief An export instance is the runtime representation of an export. It defines the export’s name and the associated
 * external value.
 * exportinst ::= {name name, value externval}
 */
typedef struct {
    char *name;
    ExternVal value;
}ExportInstance;


#ifdef __cplusplus
    }
#endif

#endif