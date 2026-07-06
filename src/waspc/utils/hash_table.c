
#include "utils/hash_table.h"



/**
 * @brief Fowler–Noll–Vo hash function 
 * 
 * @param key Null terminated string.
 * @param len Length of string.
 * @return uint32_t 
 */
uint32_t fnv(const char *key, size_t len){

    //Constant definition for FNV algoritm
    #define FNV_PRIME_32 16777619
    #define FNV_OFFSET_BASIC 2166136261u

    
    uint32_t hash = FNV_OFFSET_BASIC;    
    
    for(const char *k = key; k < (key + len); k++){
        hash ^= (uint32_t)(unsigned char)(*k);
        hash *= FNV_PRIME_32;
    }

    return hash;
}




