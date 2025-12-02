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

// #define W 18446744073709551615 // 2^64 - 1
#define W ((((unsigned __int128) 1) << 64) - 1)
#define K 32 // log(W) / 2
#define MU 4294967296 // sqrt(W) / 2
//#define D 1.4901161193847656e-08 // log(W) / sqrt(W)

uint64_t e[K];

typedef struct {
    num128 value;
    uint64_t exponent;
} kangaroo;

void fill_jump_exponents(uint64_t *jump_exponents) {
    double scale = (2.0 * MU) / (K + 1);   // floating-point

    for (uint32_t j=1; j < K+1; j++)
    {
        uint64_t e = (uint64_t)(j * scale);  
        jump_exponents[j] = j * (2 * MU) / (K+1);
    }
    
}


uint64_t pick_exponent(num128 x) {
    return e[x.s % ((unsigned __int128) K)];
}


kangaroo jump(kangaroo x) {
    kangaroo after_jump; // TODO: cant allocate on stack aaa
    
    uint64_t e_j = pick_exponent(x.value);
    num128 g_ej = gexp(e_j);

    after_jump.value = mul11585(x.value, g_ej);
    after_jump.exponent = e_j;
}

bool is_distinguished(num128 x) {
    const unsigned __int128 mask = (((unsigned __int128)1) << 26) - 1;

    return (x.s & mask) == 0;
}

// returns the discrete log
num128 dlog64(num128 target) {

    // we know x of h = g^x is in the interval W << N
    // interval width W = 2^64 - 1

    // Tame kangaroo: knows log of group element it lands on
    // wild kangaroo: knows the jumps from h
    kangaroo tame;
    tame.value = gexp(W / 2);
    tame.exponent = W / 2;

    kangaroo wild;
    wild.value = target; // TODO: maybe deep copy?
    wild.exponent = 0;

    trap traps_table[SIZE];

    bool found = false;
    while (!found)
    {
        tame_x = jump(tame_x);            
        if (is_distinguished(tame_x)) {
            // TODO: check and then insert into hash table
            uint64_t exp = lookup(traps_table, tame_x);
            if (exp != 0) {
                print("HDGKSJDgh");
                return;
            } else {
                insert(traps_table, tame_x, )
            }
            
        }

        wild_y = jump(wild_y);
        if (is_distinguished(wild_y)) {
            // TODO: check and then insert into hash table
        }

    }

}