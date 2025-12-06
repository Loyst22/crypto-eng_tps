#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>

#include "kangaroos.h"


int main()
{
    uint32_t k_tests[] = {8, 16, 32, 64, 128}; // 5
    uint32_t mu_tests[] = {500000000, 1500000000, (1 << 31), 3000000000}; // 4
    double d_tests[] = {1e-10, 5e-9, 1.5e-8, 5e-8}; // 4

    srand(time(NULL));
    setvbuf(stdout, NULL, _IONBF, 0); // dunno it wasnt printing
    printf("starting tests for dlog64():\n");
    
    for (int i = 0; i < 5; i++) {
        for (int j = 0; j < 4; j++) {
            for (int l = 0; l < 4; l++) {
            
                K = k_tests[i];
                MU = mu_tests[j];
                d = d_tests[l];

                // Repeat since it is statistical
                for (int rep = 0; rep < 1; rep++) {
                    printf("k=%u, mu=%u, d=%.1e, ", 
                        k_tests[i], 
                        mu_tests[j], 
                        d_tests[l]);

                    uint64_t x = (((uint64_t)rand() << 32) ^ (uint64_t)rand());
                    num128 target = gexp(x);

                    clock_t start = clock();

                    uint64_t result = dlog64(target);

                    clock_t elapsed = clock() - start;
                    printf("t=%.6f\n", (double)elapsed / CLOCKS_PER_SEC);
                }
            }
        }
    }


    return 0;
}
