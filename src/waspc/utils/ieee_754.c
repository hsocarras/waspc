#include "utils/ieee_754.h"
#include <string.h>

const uint8_t * DecodeIEEE754LE(const uint8_t *buffer, uint8_t n, void *val) {

    if (n == 32) {
        uint32_t bits =
            ((uint32_t)buffer[0])       |
            ((uint32_t)buffer[1] << 8)  |
            ((uint32_t)buffer[2] << 16) |
            ((uint32_t)buffer[3] << 24);

        float f;
        memcpy(&f, &bits, sizeof(f));
        *(float *)val = f;
        return buffer + 4;
    }

    if (n == 64) {
        uint64_t bits =
            ((uint64_t)buffer[0])        |
            ((uint64_t)buffer[1] << 8)   |
            ((uint64_t)buffer[2] << 16)  |
            ((uint64_t)buffer[3] << 24)  |
            ((uint64_t)buffer[4] << 32)  |
            ((uint64_t)buffer[5] << 40)  |
            ((uint64_t)buffer[6] << 48)  |
            ((uint64_t)buffer[7] << 56);

        double d;
        memcpy(&d, &bits, sizeof(d));
        *(double *)val = d;
        return buffer + 8;
    }

    /* n no soportado: no avanzar y no modificar val */
    return NULL;
}