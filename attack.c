#include "aes-128_enc.h"
#include <sys/random.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>



void invert_shift_rows(uint8_t block[AES_BLOCK_SIZE]) {
	uint8_t tmp;
  /*
	 * InvShiftRow
	 */
	/* Row 0 */
	// No changes
	/* Row 1 */
	tmp = block[13];
	block[13] = block[ 9];
	block[ 9] = block[ 5];
	block[ 5] = block[ 1];
	block[ 1] = tmp;
	/* Row 2 */
	tmp = block[2];
	block[ 2] = block[10];
	block[10] = tmp;
	tmp = block[6];
	block[ 6] = block[14];
	block[14] = tmp;
	/* Row 3 */
	tmp = block[3];
	block[ 3] = block[ 7];
	block[ 7] = block[11];
	block[11] = block[15];
	block[15] = tmp;
}

void invert_half_round(uint8_t block[AES_BLOCK_SIZE], uint8_t roundkey[AES_128_KEY_SIZE]) 
{
  // 1. AddRoundKey
  // 2. InvShiftRows
  // 3. InvSubBytes


  // AddRoundKey
  for (size_t i = 0; i < AES_BLOCK_SIZE; i++) {
    block[i] = block[i] ^ roundkey[i];
  }

  // InvShiftRows
  invert_shift_rows(block);

  // InvSubBytes
  for (size_t i = 0; i < AES_BLOCK_SIZE; i++) {
    block[i] = Sinv[block[i]];
  }

}

void print_block(uint8_t block[AES_BLOCK_SIZE]) {

  for (int i = 0; i < AES_BLOCK_SIZE; i++) {
    printf("%02x ", block[i]);
  }
}
void print_key(uint8_t key[AES_128_KEY_SIZE]) {

  for (int i = 0; i < AES_128_KEY_SIZE; i++) {
    printf("%02x ", key[i]);
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

  // TEMP: get last roundkey
  uint8_t prev_key[AES_128_KEY_SIZE];
  memcpy(prev_key, key, sizeof(uint8_t)*AES_128_KEY_SIZE);
  uint8_t next_key[AES_128_KEY_SIZE] = {0};
  
  next_aes128_round_key(prev_key, next_key, 0);
  next_aes128_round_key(next_key, prev_key, 1);
  next_aes128_round_key(prev_key, next_key, 2);
  next_aes128_round_key(next_key, prev_key, 3);
  // prev_key has the correct roundkey

  // go through all ciphertexts
  for (int i = 0; i < 256; i++) {
    invert_half_round(ciphers[i], prev_key);

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
  printf("xor of all c: \n\t");
  print_block(result);
  printf("\n");



}


