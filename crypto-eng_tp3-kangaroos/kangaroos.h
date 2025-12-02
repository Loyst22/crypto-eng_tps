#ifndef KANGAROOS_H
#define KANGAROOS_H

#include <stdint.h>
#include "mul11585.h"

#define G 4398046511104ULL

num128 gexp(uint64_t x);
num128 dlog64(num128 target);

#endif