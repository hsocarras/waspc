/**
 * @file hash.c
 * @author your name (you@domain.com)
 * @brief 
 * @version 0.1
 * @date 2024-08-16
 * 
 * @copyright Copyright (c) 2024
 * 
 */

 #include <stdint.h>


/**
 * @brief Fowler–Noll–Vo hash function 
 * 
 * @param key 
 * @param len 
 * @return uint32_t 
 */
uint32_t fnv(const char *key, uint32_t len){

    //Constant definition for FNV algoritm
    #define FNV_PRIME_32 16777619
    #define FNV_OFFSET_BASIC 2166136261u

    
    int32_t hash = FNV_OFFSET_BASIC;
    
    
    for(uint32_t i = 0; i < len; i++){
        hash ^= key[i];
        hash *= FNV_PRIME_32;
    }

    return hash;
}

/*
uint32_t CalcHashForString (const char *str, uint32_t len){

    void *src = (void)str;
    uint32_t hash;

    hash = fnv(src, len);
    
    return hash;
}*/