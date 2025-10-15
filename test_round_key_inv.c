#include "aes-128_enc.h"
#include <sys/random.h>
#include <stdio.h>

#define N_TESTS 10

int main(void) 
{   
    for (int round = 0; round < 10; round++) {
        printf("=== Starting tests for round %d ===\n", round);
        for (int i = 0; i < N_TESTS; i++) 
        {
            uint8_t prev_key[16]; 
            uint8_t next_key[16];
            if (getrandom((void *) prev_key, 16, 0) == -1) {
                printf("Error in getrandom function\n");
            }
            else {
                uint8_t verify_prev_key[16];
                next_aes128_round_key(prev_key, next_key, round);
                prev_aes128_round_key(next_key, verify_prev_key, round);

                int verify = -1;

                for (int j = 0; j < 16; j++) {
                    if (prev_key[i] != verify_prev_key[i]) {
                        verify = i;
                    }
                }
                
                if (verify != -1) {
                    printf("Error in prev_aes128_round_key function\n");
                    printf("i=%d : prev_key=%u\tverify_prev_key=%u\n", verify, prev_key[verify], verify_prev_key[verify]);
                }
                else {
                    printf("No error in prev_aes128_round_key function\n");
                }
            }
        }
        printf("\n");
    }
}

// gcc -g -Wall test_round_key_inv.c aes-128_enc.c aes-128_enc.h -o test_round_key_inv