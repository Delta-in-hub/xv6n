#ifndef __KMATCHH__
#define __KMATCHH__

#include "types.h"

static inline uint64 _clz(uint64 x) {
  uint count = 0;
  if (x == 0)
    return sizeof(x) * 8;
  while ((x >> count) != 0) {
    count++;
  }
  return sizeof(x) * 8 - count;
}

static inline uint64 log2i(uint64 x) { return sizeof(x) * 8 - _clz(x) - 1; }

static inline uint64 rounddown2i(uint64 x) { return 1 << log2i(x); }

static inline uint64 roundup2i(uint64 x) {
  uint64 t = rounddown2i(x);
  return t == x ? x : t << 1;
}

#endif
