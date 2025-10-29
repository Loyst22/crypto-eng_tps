#include "keyed_function.h"

#include <sys/random.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>

void keyed_func(uint8_t key[2 * AES_128_KEY_SIZE], uint8_t block[AES_BLOCK_SIZE]) 
{
  uint8_t k_1[AES_128_KEY_SIZE];
  uint8_t k_2[AES_128_KEY_SIZE];
  memcpy(k_1, key, sizeof(uint8_t) * AES_128_KEY_SIZE);
  memcpy(k_2, &key[AES_128_KEY_SIZE], sizeof(uint8_t) * AES_128_KEY_SIZE);

  uint8_t enc_block_1[AES_BLOCK_SIZE];
  uint8_t enc_block_2[AES_BLOCK_SIZE];
  memcpy(enc_block_1, block, sizeof(uint8_t) * AES_BLOCK_SIZE);
  memcpy(enc_block_2, block, sizeof(uint8_t) * AES_BLOCK_SIZE);

  aes128_enc(enc_block_1, k_1, 3, true);
  aes128_enc(enc_block_2, k_2, 3, true);
  
  // xor all the bytes of the two enc_block_x
  for (size_t i = 0; i < AES_BLOCK_SIZE; i++) {
    block[i] = enc_block_1[i] ^ enc_block_2[i];
  } 
}


void test_trivial_key() {
  printf("Testing trivial key on keyed_func construction\n");
  uint8_t key[2 * AES_128_KEY_SIZE] = {
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 
    0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 
    0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
  };

  uint8_t block[AES_BLOCK_SIZE] = {
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 
    0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10
  };

  // print key
  printf("k: ");
  for (int i = 0; i < 2*AES_BLOCK_SIZE; i++) {
    printf("%02x", key[i]);
  }
  printf("\n");
  
  // Print original data block
  printf("m: ");
  for (int i = 0; i < AES_BLOCK_SIZE; i++) {
    printf("%02x", block[i]);
  }
  printf("\n");

  keyed_func(key, block);


  // Print ciphered block
  printf("c: ");
  for (int i = 0; i < AES_BLOCK_SIZE; i++) {
    printf("%02x", block[i]);
  }
  printf("\n");
}

