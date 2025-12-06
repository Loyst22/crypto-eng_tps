#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "mul11585.h"

#define SIZE (1 << 20)
#define NOT_FOUND UINT64_MAX

typedef struct {
    num128 key;
    uint64_t exponent;
} trap;

void insert(trap *ht, const num128 key, uint64_t exponent) {
    uint32_t index = (key.t[0] ^ (key.t[1] << 7)) % SIZE;

    ht[index].key = key;
    ht[index].exponent = exponent;
}

uint64_t lookup(trap *ht, num128 key) {
    uint32_t index = (key.t[0] ^ (key.t[1] << 7)) % SIZE;

    if (ht[index].key.s == key.s)
        return ht[index].exponent;

    return NOT_FOUND;
}