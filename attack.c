#include "aes-128_enc.h"
#include <sys/random.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>


// void invert_half_round(uint8_t plaintext[AES_BLOCK_SIZE], uint8_t ciphertext[AES_BLOCK_SIZE], uint8_t roundkey[AES_128_KEY_SIZE]) 
// {
//   // 1. InvShiftRows
//   // 2. InvSubBytes
//   // 3. AddRoundKey
//
//
// }

void print_block(uint8_t block[AES_BLOCK_SIZE]) {

  for (int i = 0; i < AES_BLOCK_SIZE; i++) {
    printf("%02x", block[i]);
  }
}

int main(void)
{
  uint8_t key[AES_128_KEY_SIZE] = {
    0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 
    0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10,
  };

  uint8_t ciphers[256][AES_BLOCK_SIZE] = {0};
  uint8_t plaintext[AES_BLOCK_SIZE] = {0};
  uint8_t result[AES_BLOCK_SIZE] = {0};

  // plaintext = {i, 0x00, ..., 0x00}
  for (size_t i = 0; i < 256; i++) {
    plaintext[0] = i;
    memcpy(ciphers[i], plaintext, sizeof(uint8_t)*AES_BLOCK_SIZE);

    aes128_enc(ciphers[i], key, 4, false);
  }


  // go through all ciphertexts
  for (int i = 0; i < 256; i++) {
    
    // XOR all bytes
    for (size_t byte = 0; byte < AES_BLOCK_SIZE; byte++) {
      result[byte] = result[byte] ^ ciphers[i][byte];
    } 
  }

  printf("first two ciphertexts: \n\t");
  print_block(ciphers[0]);
  printf("\n\t");
  print_block(ciphers[1]);
  printf("\n");

  // should return all zeros 
  printf("xor of all c: ");
  print_block(result);
  printf("\n");



}


