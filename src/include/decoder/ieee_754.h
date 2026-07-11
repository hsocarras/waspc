#ifndef WASPC_UTILS_IEEE_754_H
#define WASPC_UTILS_IEEE_754_H

#ifdef __cplusplus
    extern "C" {
#endif

#include <stdint.h>

const uint8_t * DecodeIEEE754LE(const uint8_t *buffer, uint8_t n, void *val);

#define DecodeF32(buff, val) DecodeIEEE754LE(buff, 32, (void *) val)
#define DecodeF64(buff, val) DecodeIEEE754LE(buff, 64, (void *) val)



#ifdef __cplusplus
    }
#endif

#endif