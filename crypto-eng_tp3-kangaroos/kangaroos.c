#include <stdbool.h>
#include "kangaroos.h"


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
            result = mul11585(result, base); // already does mod?
        }
        exp = exp >> 1;
        base = mul11585(base, base);
    }
    
    return result;
}


// bool test_gexp()
// {
//     bool verify = false;

//     return verify;
// }


// int main(void)
// {



//     return 0;
// }