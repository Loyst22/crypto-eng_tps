#ifndef KANGAROOS_H
#define KANGAROOS_H

#include <stdint.h>
#include "mul11585.h"

#define G 4398046511104ULL

#define W ((((unsigned __int128) 1) << 64) - 1)

// Global variables for tweaking parameters
extern uint32_t K;
extern uint64_t MU;
extern double d;  // Distinguished point probability (1/d)

num128 gexp(uint64_t x);
uint64_t dlog64(num128 target);

#endif