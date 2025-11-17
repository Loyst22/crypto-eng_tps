#ifndef SECOND_PREIM_H
#define SECOND_PREIM_H

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

#define MASK_24 0xFFFFFFULL
#define CONV_24_to_48(x) ( ((uint64_t)(x[0] & MASK_24)) | ((uint64_t)(x[1] & MASK_24) << 24) )
#define MASK_48 0xFFFFFFFFFFFFULL

#define N (1 << 24)
#define N_BLOCKS (1 << 18)

#define ORIGINAL_HASH 0x7CA651E182DBULL

void speck48_96(const uint32_t k[4], const uint32_t p[2], uint32_t c[2]);
void speck48_96_inv(const uint32_t k[4], const uint32_t c[2], uint32_t p[2]);
uint64_t cs48_dm(const uint32_t m[4], const uint64_t h);
uint64_t hs48(const uint32_t *m, uint64_t fourlen, int padding, int verbose);

uint64_t get_cs48_dm_fp(uint32_t m[4]);
void find_exp_mess(uint32_t m1[4], uint32_t m2[4]);
bool attack(void);

#endif // !SECOND_PREIM_H


