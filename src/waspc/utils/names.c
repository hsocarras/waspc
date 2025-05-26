
#include <stdlib.h>
#include <string.h>
#include "utils/names.h"

/**
 * @brief Converts a Name struct to a null-terminated C string.
 *
 * Allocates a new string and copies the contents of the Name struct,
 * ensuring the result is null-terminated. The caller is responsible
 * for freeing the returned string.
 *
 * @param name The Name struct to convert.
 * @return char* A newly allocated null-terminated C string.
 */
char* WasNameToString(Name name) {
    char* str = (char*)malloc(name.lenght + 1);
    if (!str) return NULL;
    memcpy(str, name.name, name.lenght);
    str[name.lenght] = '\0';
    return str;
}

/**
 * @brief Converts a null-terminated C string to a Name struct.
 *
 * Allocates memory for the Name struct's data and copies the string.
 * The caller is responsible for freeing the allocated memory in name.name.
 *
 * @param str The null-terminated C string to convert.
 * @return Name The resulting Name struct.
 */
Name StrToWasName(const char* str) {
    Name name;
    name.lenght = (uint32_t)strlen(str);
    name.name = (char*)malloc(name.lenght);
    if (name.name) {
        memcpy(name.name, str, name.lenght);
    }
    return name;
}