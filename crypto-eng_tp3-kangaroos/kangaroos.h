#ifndef KANGAROOS_H
#define KANGAROOS_H

#include <stdint.h>
#include "mul11585.h"

#define G 4398046511104ULL

#define W ((((unsigned __int128) 1) << 64) - 1)
#define K 32 // log(W) / 2
#define MU ((unsigned __int128)(1ULL << 31)) // sqrt(W) / 2



num128 gexp(uint64_t x);
num128 dlog64(num128 target);

#endif