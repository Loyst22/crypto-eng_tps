#include "aes-128_enc.h"
#include "attack.h"
#include "keyed_function.h"
#include <stdio.h>
#include <stdbool.h>
#include <sys/random.h>
#include <string.h>

#define N_TESTS 10

uint32_t test_recover_key() {
  uint8_t key[AES_128_KEY_SIZE] = {
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 
    0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
  };

  uint8_t roundkey[AES_128_KEY_SIZE] = {
    0xfe, 0x2c, 0xf7, 0x96, 0x44, 0xd5, 0xb4, 0x6e, 
    0x66, 0x26, 0xf3, 0xe5, 0x76, 0xa9, 0x7a, 0xda
  };

  invert_key(roundkey);
  
  for (uint32_t i = 0; i < AES_128_KEY_SIZE; i++) {
    if (roundkey[i] != key[i]) {
      return false;
    }
  }
  return true;
}

uint32_t test_keyed_func_distinguisher() {
  printf("Running 3-round distinguisher on keyed function construction:\n");

	uint8_t key[2 * AES_128_KEY_SIZE] = {0};
	generate_random_key(key);
  generate_random_key(&key[AES_128_KEY_SIZE]);

  uint8_t result[AES_BLOCK_SIZE] = {0};
  keyed_func_distinguisher(key, result);

	printf("key: \n");
	print_key(key);
	print_key(&key[AES_128_KEY_SIZE]);
  printf("XOR of all Ciphers\n");
  print_block(result);
  printf("\n");

  return 0;
}


uint32_t test_round_key_inv() {
  uint32_t errors = 0;

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
                errors += 1;
            }
            else {
                printf("No error in prev_aes128_round_key function\n");
            }
        }
    }
    printf("\n");
  }

  return errors;
}


uint32_t attack_normal_aes() {
  printf("Running 3-round distinguisher attack on normal AES and random key:\n");

	uint8_t key[AES_128_KEY_SIZE] = {0};
	generate_random_key(key);

	uint8_t ciphers[AES_BLOCK_SIZE][256][AES_BLOCK_SIZE] = {0};
	uint8_t recovered_key[AES_128_KEY_SIZE] = {0};

	gen_all_delta_sets(ciphers, key);
	key_recovery_attack(ciphers, recovered_key);

	printf("Actual key: \n");
	print_key(key);
	printf("Recovered key: \n");
	print_key(recovered_key);
  printf("\n");

  // return memcmp(key, recovered_key, sizeof(uint8_t)*AES_128_KEY_SIZE);
  return 0;
}

uint32_t attack_alt_xtime_aes() {
  printf("Running 3-round distinguisher attack on alt xtime AES and random key:\n");

	uint8_t key[AES_128_KEY_SIZE] = {0};
	generate_random_key(key);

	uint8_t ciphers[AES_BLOCK_SIZE][256][AES_BLOCK_SIZE] = {0};
	uint8_t recovered_key[AES_128_KEY_SIZE] = {0};

    // Encrypt using different AES
	gen_all_delta_sets_alt_xtime(ciphers, key);
	key_recovery_attack(ciphers, recovered_key);

	printf("Actual key: \n");
	print_key(key);
	printf("Recovered key: \n");
	print_key(recovered_key);
  printf("\n");

  // return memcmp(key, recovered_key, sizeof(uint8_t)*AES_128_KEY_SIZE);
  return 0;
}

uint32_t attack_alt_sbox_aes() {
  printf("Running 3-round distinguisher attack on alt S-box AES and random key:\n");

	uint8_t key[AES_128_KEY_SIZE] = {0};
	generate_random_key(key);

	uint8_t ciphers[AES_BLOCK_SIZE][256][AES_BLOCK_SIZE] = {0};
	uint8_t recovered_key[AES_128_KEY_SIZE] = {0};

    // Encrypt using different AES
	gen_all_delta_sets_alt_sbox(ciphers, key);
	key_recovery_attack_alt_sbox(ciphers, recovered_key);

	printf("Actual key: \n");
	print_key(key);
	printf("Recovered key: \n");
	print_key(recovered_key);
  printf("\n");

  // return memcmp(key, recovered_key, sizeof(uint8_t)*AES_128_KEY_SIZE);
  return 0;
}

uint32_t attack_alt_aes() {
  printf("Running 3-round distinguisher attack on alt S-box AES and random key:\n");

	uint8_t key[AES_128_KEY_SIZE] = {0};
	generate_random_key(key);

	uint8_t ciphers[AES_BLOCK_SIZE][256][AES_BLOCK_SIZE] = {0};
	uint8_t recovered_key[AES_128_KEY_SIZE] = {0};

    // Encrypt using different AES
	gen_all_delta_sets_alt_sbox(ciphers, key);
	key_recovery_attack_alt_sbox(ciphers, recovered_key);

	printf("Actual key: \n");
	print_key(key);
	printf("Recovered key: \n");
	print_key(recovered_key);
  printf("\n");

  // return memcmp(key, recovered_key, sizeof(uint8_t)*AES_128_KEY_SIZE);
  return 0;
}

uint32_t compare_aes_ciphers() {
  printf("Testing alt AES variations for same key and plaintext\n");

	uint8_t key[AES_128_KEY_SIZE] = {0};
	generate_random_key(key);

  uint8_t block_0[AES_BLOCK_SIZE] = {
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 
    0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
  };
  uint8_t block_1[AES_BLOCK_SIZE] = {};
  uint8_t block_2[AES_BLOCK_SIZE] = {};
  memcpy(block_1, block_0, sizeof(uint8_t)*AES_BLOCK_SIZE);
  memcpy(block_2, block_0, sizeof(uint8_t)*AES_BLOCK_SIZE);

  aes128_enc(block_0, key, 4, false);
  aes128_enc_alt_xtime(block_1, key, 4, false);
  aes128_enc_alt_sbox(block_2, key, 4, false);

  printf("Key:\n");
  print_key(key);
  printf("Normal AES ciphered block:\n");
  print_block(block_0);

  printf("Alt XTIME AES ciphered block:\n");
  print_block(block_1);
  printf("Alt S-box AES ciphered block:\n");
  print_block(block_2);
  printf("\n");

  return 0;
}

int main(void)
{
  printf("-------------------Keyed Function Construction---------------------------\n");
  test_trivial_key();
  test_keyed_func_distinguisher();

  printf("-------------------Comparing ciphertext of all cipher variations---------------------------\n");
  compare_aes_ciphers();
  printf("-------------------Running attack on all cipher variations---------------------------------\n");

  attack_normal_aes();
 
  attack_alt_xtime_aes();
  attack_alt_sbox_aes();

}
