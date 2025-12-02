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
static num128 jumps[K];

typedef struct {
    num128 value;
    uint64_t exponent;
} kangaroo;

void fill_jump_exponents() {
    for (uint32_t j=1; j <= K; j++)
    {
        uint64_t exponent = ((2*j - 1) * MU) / K;  
        e[j-1] = exponent;

        num128 jump = gexp(exponent);
        jumps[j-1] = jump;
    }
}

uint32_t jump_index(num128 x) {
    uint32_t index = x.t[0] & (0b11111ULL); // take lowest 5 bits of x
    return index;
}

void jump(kangaroo *x) {
    // kangaroo after_jump; // TODO: cant allocate on stack aaa
    
    uint32_t j = jump_index(x->value);
    uint64_t e_j = e[j];
    num128 g_ej = jumps[j];

    x->value = mul11585(x->value, g_ej);
    x->exponent += e_j;
}

bool is_distinguished(kangaroo roo) {
    // const unsigned __int128 mask = (((unsigned __int128)1) << 26) - 1;
    const unsigned __int128 mask = (((unsigned __int128)1) << 20) - 1;

    return (roo.value.s & mask) == 0;
}

num128 n_abs(kangaroo roo, uint64_t stored_exp) {
    num128 result;
    result.s = (roo.exponent > stored_exp) ? 
        (roo.exponent - stored_exp) : 
        (stored_exp - roo.exponent);
    return result;
}

// returns the discrete log
num128 dlog64(num128 target) {
    // we know x of h = g^x is in the interval W << N
    // interval width W = 2^64 - 1
    fill_jump_exponents();

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
                return n_abs(tame, exp);

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
                return n_abs(wild, exp);

            } else {
                insert(traps_table, wild.value, wild.exponent);
            }
            
        }

    }

    // return; // should not be reachable?

}