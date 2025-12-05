#include <string.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdio.h>
#include "kangaroos.h"
#include "hasht.h"

/**
 * @brief Implements the exponentiation map x → g^x where g is a generator of the group G.
 * @param x The exponent we wish to apply on g
 * @return num128 - The element g^x in G.
 */
num128 gexp(uint64_t x) 
{
    // https://en.wikipedia.org/wiki/Modular_exponentiation
    uint64_t exp = x;
    num128 base = {.s = G};
    num128 result;
    result.s = 1;

    while (exp > 0)
    {
        if (exp & 1) {
            result = mul11585(result, base); // already does mod
        }
        exp = exp >> 1;
        base = mul11585(base, base);
    }
    
    return result;
}

static uint64_t E[K];
static num128 Jumps[K];

// Structure to store distinguished points with their exponents
typedef struct {
    num128 value;      // The group element
    uint64_t exponent;  // The exponent (log value)
    bool occupied;     // Whether this slot is used
} distinguished_point;

// Arrays to store distinguished points
static distinguished_point tame_points[SIZE];
static distinguished_point wild_points[SIZE];
static size_t tame_count = 0;
static size_t wild_count = 0;

typedef struct {
    num128 value;
    uint64_t exponent;
} kangaroo;

void fill_jump_exponents() {
    for (uint32_t j=0; j < K; j++)
    {
        uint64_t exponent = (((uint64_t)rand() << 32) ^ (uint64_t)rand()) & 0xFFFFFFFFULL;
        while (exponent == 0)
        {
            exponent = (((uint64_t)rand() << 32) ^ (uint64_t)rand()) & 0xFFFFFFFFULL;
        }
        
        E[j] = exponent;
        Jumps[j] = gexp(exponent);
        
        /*
        uint64_t exponent = ((2*j - 1) * MU) / K;  
        E[j-1] = exponent;

        num128 jump = gexp(exponent);
        Jumps[j-1] = jump;
        */
    }
}

uint32_t jump_index(num128 x) {
    uint32_t index = x.t[0] ^ x.t[1];
    return (index & (K-1)); // take lowest 5 bits of fingerprint of x
}

void jump(kangaroo *x) {  
    uint64_t j = jump_index(x->value);
    uint64_t e_j = E[j];
    num128 g_ej = Jumps[j];

    x->value = mul11585(x->value, g_ej);
    x->exponent += e_j;
}

bool is_distinguished(kangaroo roo) {
    const unsigned __int128 mask = (((unsigned __int128)1) << 26) - 1;
    return (roo.value.t[0] & mask) == 0;
}

// Compare two num128 values for equality
bool num128_equal(num128 a, num128 b) {
    return a.s == b.s;
}

// Search for a value in the tame array
// Returns the exponent if found, -1 if not found
uint64_t lookup_tame(num128 value) {
    for (size_t i = 0; i < tame_count; i++) {
        if (tame_points[i].occupied && num128_equal(tame_points[i].value, value)) {
            return tame_points[i].exponent;
        }
    }
    return UINT64_MAX;
}

// Search for a value in the wild array
// Returns the exponent if found, -1 if not found
uint64_t lookup_wild(num128 value) {
    for (size_t i = 0; i < wild_count; i++) {
        if (wild_points[i].occupied && num128_equal(wild_points[i].value, value)) {
            return wild_points[i].exponent;
        }
    }
    return UINT64_MAX;
}

// Insert into tame array
void insert_tame(num128 value, __int128_t exponent) {
    if (tame_count >= SIZE) {
        fprintf(stderr, "Error: Tame array full!\n");
        return;
    }
    tame_points[tame_count].value = value;
    tame_points[tame_count].exponent = exponent;
    tame_points[tame_count].occupied = true;
    tame_count++;
}

// Insert into wild array
void insert_wild(num128 value, __int128_t exponent) {
    if (wild_count >= SIZE) {
        fprintf(stderr, "Error: Wild array full!\n");
        return;
    }
    wild_points[wild_count].value = value;
    wild_points[wild_count].exponent = exponent;
    wild_points[wild_count].occupied = true;
    wild_count++;
}

// Returns the discrete log
num128 dlog64(num128 target) {
    // we know x of h = g^x is in the interval W << N
    // interval width W = 2^64 - 1
    fill_jump_exponents();

    // Initialize arrays
    tame_count = 0;
    wild_count = 0;
    memset(tame_points, 0, sizeof(distinguished_point) * SIZE);
    memset(wild_points, 0, sizeof(distinguished_point) * SIZE);

    // Tame kangaroo: starts at g^(W/2)
    kangaroo tame;
    tame.value = gexp(0); // gexp(W / 2);
    tame.exponent = 0; // W / 2;

    // Wild kangaroo: starts at h = g^x (target)
    kangaroo wild;
    wild.value = target;
    wild.exponent = 0;  // Tracks jumps from h

    while (true)
    {
        // Tame kangaroo jump
        jump(&tame);            
        if (is_distinguished(tame)) {
            // Check if wild kangaroo has been here
            uint64_t wild_exp = lookup_wild(tame.value);
            
            // printf("Tame found distinguished point at exp=%lu\n", tame.exponent);
            
            if (wild_exp != UINT64_MAX) {  // Found collision!
                // At collision: g^(tame.exponent) = h * g^(wild_exp)
                // Therefore: x = tame.exponent - wild_exp
                printf("COLLISION! tame_exp=%lu, wild_exp=%lu\n", tame.exponent, wild_exp);
                num128 result;
                result.s = tame.exponent - wild_exp;
                return result;
            } else {
                insert_tame(tame.value, tame.exponent);
            }
        }

        // Wild kangaroo jump
        jump(&wild);            
        if (is_distinguished(wild)) {
            // Check if tame kangaroo has been here
            uint64_t tame_exp = lookup_tame(wild.value);
            
            // printf("Wild found distinguished point at exp=%lu\n", wild.exponent);
            
            if (tame_exp != UINT64_MAX) {  // Found collision!
                // At collision: g^(tame_exp) = h * g^(wild.exponent)
                // Therefore: x = tame_exp - wild.exponent
                printf("COLLISION! tame_exp=%lu, wild_exp=%lu\n", tame_exp, wild.exponent);
                num128 result;
                result.s = tame_exp - wild.exponent;
                return result;
            } else {
                insert_wild(wild.value, wild.exponent);
            }
        }
        
        // Safety check to prevent infinite loops during testing
        if (tame_count + wild_count > SIZE - 10) {
            fprintf(stderr, "Warning: Approaching array capacity limits\n");
        }
    }
}