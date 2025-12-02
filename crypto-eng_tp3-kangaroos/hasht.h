#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include "mul11585.h"

#define SIZE (1 << 8)

typedef struct {
    num128 key;
    uint64_t exponent;
} trap;

void insert(trap *ht, const num128 key, uint64_t exponent) {
    uint32_t index = key.s % SIZE;

    ht[index].key = key;
    ht[index].exponent = exponent;
}

uint64_t lookup(trap *ht, num128 key) {
    uint32_t index = key.s % SIZE;

    if (ht[index].key.s == key.s)
        return ht[index].exponent;

    return UINT64_MAX;
}