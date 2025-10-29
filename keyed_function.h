#ifndef KEYED_FUNC_H
#define KEYED_FUNC_H

#include "aes-128_enc.h"

void keyed_func(uint8_t key[2*AES_128_KEY_SIZE], uint8_t block[AES_BLOCK_SIZE]);

void test_trivial_key();

#endif // !KEYED_FUNC_H

