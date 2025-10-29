#ifndef ATTACK_H
#define ATTACK_H

// recover initial AES-128 key from roundkey 4
void invert_key(uint8_t key[AES_128_KEY_SIZE]);

void print_block(uint8_t block[AES_BLOCK_SIZE]);
void print_key(uint8_t key[AES_128_KEY_SIZE]);

void generate_random_key(uint8_t key[AES_128_KEY_SIZE]);


void gen_delta_set_and_cipher(uint8_t ciphers[256][AES_BLOCK_SIZE], uint8_t varying_byte, uint8_t key[AES_128_KEY_SIZE]);
void gen_all_delta_sets(uint8_t ciphers[AES_BLOCK_SIZE][256][AES_BLOCK_SIZE], uint8_t key[AES_128_KEY_SIZE]);

void gen_all_delta_sets_alt_xtime(uint8_t ciphers[AES_BLOCK_SIZE][256][AES_BLOCK_SIZE], uint8_t key[AES_128_KEY_SIZE]);
void gen_all_delta_sets_alt_sbox(uint8_t ciphers[AES_BLOCK_SIZE][256][AES_BLOCK_SIZE], uint8_t key[AES_128_KEY_SIZE]);


void key_recovery_attack(uint8_t ciphers[AES_BLOCK_SIZE][256][AES_BLOCK_SIZE], uint8_t key_guess[AES_128_KEY_SIZE]);
void key_recovery_attack_alt_sbox(uint8_t ciphers[AES_BLOCK_SIZE][256][AES_BLOCK_SIZE], uint8_t key_guess[AES_128_KEY_SIZE]);

int keyed_func_distinguisher(uint8_t key[2 * AES_128_KEY_SIZE], uint8_t result[AES_BLOCK_SIZE]);

#endif // !ATTACK_H
