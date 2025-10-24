#include "aes-128_enc.h"
#include <sys/random.h>
#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <fcntl.h>
#include <unistd.h>

/************************************************************/
/*
 * 						Print helpers
 */
/************************************************************/

void print_block(uint8_t block[AES_BLOCK_SIZE]) {
	for (int i = 0; i < AES_BLOCK_SIZE; i++) {
    	printf("%02x ", block[i]);
	}
	printf("\n");
}

void print_key(uint8_t key[AES_128_KEY_SIZE]) {
	for (int i = 0; i < AES_128_KEY_SIZE; i++) {
    	printf("%02x ", key[i]);
  	}
	printf("\n");
}

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

/**
 * Performs the inverse of AES cipher for the last (half) round consisting of 
 * - SubBytes
 * - ShiftRows
 * - AddRoundKey
 */
void invert_half_round(uint8_t block[AES_BLOCK_SIZE], uint8_t roundkey[AES_128_KEY_SIZE]) 
{
	// AddRoundKey
	for (size_t i = 0; i < AES_BLOCK_SIZE; i++) {
		block[i] = block[i] ^ roundkey[i];
	}

	// InvShiftRows
	// invert_shift_rows(block); // this is actually unnecessary for what we're doing here

	// InvSubBytes
	for (size_t i = 0; i < AES_BLOCK_SIZE; i++) {
		block[i] = Sinv[block[i]];
  	}
}

/**
 * Unused (other way of computing the key)
 */
int invert_half_round_per_byte(uint8_t block_byte, uint8_t key_byte) 
{
	return Sinv[block_byte ^ key_byte] ;
}

/**
 * @brief
 * Performs the inverse opération of next round key 4 times (rounds 3, 2, 1, 0) for a given key 
 */
void invert_key(uint8_t key[AES_128_KEY_SIZE]) {
	uint8_t tmp_key[AES_128_KEY_SIZE];
	prev_aes128_round_key(key, tmp_key, 3);
	prev_aes128_round_key(tmp_key, key, 2);
	prev_aes128_round_key(key, tmp_key, 1);
	prev_aes128_round_key(tmp_key, key, 0);
}


/**
 * @brief
 * Generates a random key from /dev/urandom
 */
void generate_random_key(uint8_t key[AES_128_KEY_SIZE]) {
    int fd = open("/dev/urandom", O_RDONLY);
    read(fd, key, AES_128_KEY_SIZE);
    close(fd);
}

/**
 * @brief
 * Generates a delta set with one byte taking all possible values (varying_byte)
 * while the 15 others are constant
 * Then ciphers all the delta set according to the key provided
 */
void gen_delta_set_and_cipher(uint8_t ciphers[256][AES_BLOCK_SIZE], uint8_t varying_byte, uint8_t key[AES_128_KEY_SIZE]) {
	uint8_t plaintext[AES_BLOCK_SIZE] = {0};

	for (size_t i = 0; i < 256; i++) {
		plaintext[varying_byte] = i;
		memcpy(ciphers[i], plaintext, sizeof(uint8_t)*AES_BLOCK_SIZE);
	
		aes128_enc(ciphers[i], key, 4, false);
	}
}

/**
 * @brief
 * Generates all delta sets
 * then cipher them all according to the key provided
 */
void gen_all_delta_sets(uint8_t ciphers[AES_BLOCK_SIZE][256][AES_BLOCK_SIZE], uint8_t key[AES_128_KEY_SIZE]) {
	for (size_t varying_byte = 0; varying_byte < AES_BLOCK_SIZE; varying_byte++) {
		gen_delta_set_and_cipher(ciphers[varying_byte], varying_byte, key);
	}
}

void compute_distinguisher_for_key_guess(uint8_t ciphers[256][AES_BLOCK_SIZE], uint8_t key_guess[AES_128_KEY_SIZE], uint8_t result[AES_BLOCK_SIZE]) {
	memset(result, 0, AES_BLOCK_SIZE);
	// print_key(key_guess);
	
	// go through all ciphertexts
	for (size_t i = 0; i < 256; i++) {
		invert_half_round(ciphers[i], key_guess);
		
    	// XOR block by block
    	for (size_t byte = 0; byte < AES_BLOCK_SIZE; byte++) {
  			result[byte] = result[byte] ^ ciphers[i][byte];
		}
	}

}

/**
 * Unused (other way of computing the key)
 */
int compute_distinguisher_for_key_guess_byte(uint8_t ciphers[256][AES_BLOCK_SIZE], uint8_t key_guess[AES_128_KEY_SIZE], uint8_t i_byte) {
	int acc = 0;

	// go through all ciphertexts
	for (size_t i = 0; i < 256; i++) {
		acc ^= invert_half_round_per_byte(ciphers[i][i_byte], key_guess[i_byte]);
	}

	return acc;
}

/**
 * @brief 
 * Checks if the distinguisher computed for a delta set indicates that the guess is a key candidate
 * 
 * Check the validity of this, it may be false 
 */
bool verify_result(uint8_t result[AES_BLOCK_SIZE], uint8_t delta_set) {
	if (result[delta_set] == 0) {
		return true;
	}
	return false;
}

/**
 * @brief
 * Performs the whole attack.
 * 
 * @param ciphers
 * The encrypted chosen plaintexts (the function relies on a first encryption and then may re-encrypt the plaintexts).
 * @param key
 * The actual key used to cipher the chose plaintexts.
 * 
 */
void key_recovery_attack(uint8_t ciphers[AES_BLOCK_SIZE][256][AES_BLOCK_SIZE], uint8_t key_guess[AES_128_KEY_SIZE]) {
	uint8_t ciphers_copy[AES_BLOCK_SIZE][256][AES_BLOCK_SIZE];
	memcpy(ciphers_copy, ciphers, sizeof(uint8_t)*AES_BLOCK_SIZE*256*AES_BLOCK_SIZE);
	uint8_t result[AES_BLOCK_SIZE] = {0};

	/**
	 * For each byte of the key
	 * 		- For each possible byte
	 * 			-  Compute distinguisher
	 * 				- If result == 0:
	 * 					- Try all DELTA-sets (avoid false positives)
	 * 						- If all 0: GOOD KEY BYTE & Go to next byte of the key
	 * 						- If not: WRONG KEY & Go to next possible value
	 * 				- If not: WRONG KEY & Go to next possible value
	 * 		- Return found key
	 */
	for (size_t keybyte_index = 0; keybyte_index < AES_128_KEY_SIZE; keybyte_index++) {
		bool byte_found = false;

		for (size_t guess = 0; guess < 256 && !byte_found; guess++) {
			// reset ciphertexts
			memcpy(ciphers, ciphers_copy, sizeof(uint8_t)*AES_BLOCK_SIZE*256*AES_BLOCK_SIZE);
			// make a guess
			key_guess[keybyte_index] = guess;

			// Try all DELTA-sets (avoid false positives)
			bool correct_key = true;
			for (size_t delta_set = 0; delta_set < AES_BLOCK_SIZE; delta_set++) {
				compute_distinguisher_for_key_guess(ciphers[delta_set], key_guess, result);

				if (!verify_result(result, keybyte_index)) {
					// WRONG KEY & Go to next possible value
					correct_key = false;
					break;
				}
			}
			// GOOD KEY BYTE & Go to next byte of the key
			if (correct_key) {
				byte_found = true;
			}
		}
	}

	invert_key(key_guess);
}


int main(void)
{
	uint8_t key[AES_128_KEY_SIZE] = {0};
	generate_random_key(key);

	uint8_t ciphers[AES_BLOCK_SIZE][256][AES_BLOCK_SIZE] = {0};
	uint8_t recovered_key[AES_128_KEY_SIZE] = {0};
	uint8_t key_guess[AES_128_KEY_SIZE] = {0};
	uint8_t result[AES_BLOCK_SIZE] = {0};

	gen_all_delta_sets(ciphers, key);
	key_recovery_attack(ciphers, recovered_key);

	printf("Actual key: \n");
	print_key(key);
	printf("Recovered key: \n");
	print_key(recovered_key);
}
