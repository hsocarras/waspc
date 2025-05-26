

#ifndef WASPC_UTILS_NAMES_H
#define WASPC_UTILS_NAMES_H

#ifdef __cplusplus
extern "C" {
#endif

#include <string.h>
#include "webassembly/structure/types.h"

//Convert a Name struct to null-terminated C string.
char* WasNameToString(Name name);

//Convert a null-terminated C string to a Name struct.
Name StrToWasName(const char* str);

#ifdef __cplusplus
}
#endif

#endif // UTILS_NAMES_H