#include <string.h>
#include <stdbool.h>
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

//#define D 1.4901161193847656e-08 // log(W) / sqrt(W)

static uint64_t e[K];

typedef struct {
    num128 value;
    uint64_t exponent;
} kangaroo;

void fill_jump_exponents(uint64_t *jump_exponents) {
    for (uint32_t j=1; j <= K; j++)
    {
        uint64_t e = ((2*j - 1) * MU) / K;  
        jump_exponents[j-1] = e;
    }
}

uint64_t pick_exponent(num128 x) {
    uint64_t index = x.t[0] & (0b11111ULL); // take lowest 5 bits of x
    return e[index];
}

void jump(kangaroo *x) {
    // kangaroo after_jump; // TODO: cant allocate on stack aaa
    
    uint64_t e_j = pick_exponent(x->value);
    num128 g_ej = gexp(e_j);

    x->value = mul11585(x->value, g_ej);
    x->exponent += e_j;
}

bool is_distinguished(kangaroo roo) {
    const unsigned __int128 mask = (((unsigned __int128)1) << 26) - 1;

    return (roo.value.s & mask) == 0;
}

num128 n_abs(kangaroo a, kangaroo b) {
    num128 result;
    result.s = (a.exponent > b.exponent) ? 
        (a.exponent - b.exponent) : 
        (b.exponent - a.exponent);
    return result;
}

// returns the discrete log
num128 dlog64(num128 target) {
    // we know x of h = g^x is in the interval W << N
    // interval width W = 2^64 - 1
    fill_jump_exponents(e);

    // Tame kangaroo: knows log of group element it lands on
    // wild kangaroo: knows the jumps from h
    kangaroo tame;
    tame.value = gexp(W / 2);
    tame.exponent = W / 2;

    kangaroo wild;
    wild.value = target; // TODO: maybe deep copy?
    wild.exponent = 0;

    trap traps_table[SIZE];
    memset(traps_table, 0, sizeof(trap) * SIZE);

    bool found = false;
    while (!found)
    {
        jump(&tame);            
        if (is_distinguished(tame)) {

            // TODO: check and then insert into hash table
            uint64_t exp = lookup(traps_table, tame.value);

                // UINT_MAX -> not found
            if (exp != UINT64_MAX) {
                return n_abs(tame, wild);

            } else {
                insert(traps_table, tame.value, tame.exponent);
            }
            
        }

        jump(&wild);            
        if (is_distinguished(wild)) {

            // TODO: check and then insert into hash table
            uint64_t exp = lookup(traps_table, wild.value);

                // UINT_MAX -> not found
            if (exp != UINT64_MAX) {
                return n_abs(tame, wild);

            } else {
                insert(traps_table, wild.value, wild.exponent);
            }
            
        }

    }

    // return; // should not be reachable?

}