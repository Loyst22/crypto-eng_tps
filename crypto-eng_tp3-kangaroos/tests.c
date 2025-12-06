#include <stdint.h>
#include <stdbool.h>
#include "kangaroos.h"

bool test_gexp() 
{
    #define N_TESTS 3

        // { LOWER, UPPER} little endian my behated
    static const num128 expected_res[N_TESTS] = {
        { .t = { 0x0840EE23EECF13E4ULL, 0x042F953471EDC0ULL } },
        { .t = { 0xD8BC716B91D838CCULL, 0x21F33CAEB45F4ULL } },
        { .t = { 0x25357DAACBF4868FULL, 0x7A2A1DEC09D03ULL } }
    };

    static const uint64_t exp[N_TESTS] = {
        257ULL,
        112123123412345ULL,
        18014398509482143ULL
    };


    num128 actual_res;
    bool result = true;

    for (size_t i = 0; i < N_TESTS; i++)
    {
        actual_res = gexp(exp[i]);
        if (actual_res.s != expected_res[i].s)
        {
            printf("gexp(%lu) failed\n", exp[i]);
            printf("    expected: 0x");
            print_num128(expected_res[i]);
            printf("\n");
            printf("    actual:   0x");
            print_num128(actual_res);
            printf("\n");

            result = false;
        }
    }
    
    return result;
}

int main()
{
    printf("Testing gexp():\n");
    if (test_gexp()) {
        printf("    gexp() correct ✅\n");
    } else {
        printf("    gexp NOT correct ❌\n");
    }

    printf("Testing dlog64():\n");

    num128 target = { .t = { 0xB6263BF2908A7B09ULL, 0x71AC72AF7B138ULL } };
    uint64_t result = dlog64(target);

    num128 exp = gexp(result);
    printf("  result: ");
    print_num128(exp);
    printf("\n");

    printf("expected: ");
    print_num128(target);
    printf("\n");

    return 0;
}
