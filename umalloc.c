// Implementations of umalloc
// use sbrk() to allocate memory from the heap

#include "umalloc.h"
#include <assert.h>
#include <unistd.h>

// return next power of 2
static inline size_t round_up(size_t x) {
  if (x < 2) {
    return 1;
  } else {
    return 1 << (8 * sizeof(x) - __builtin_clz(x - 1));
  }
}

#define MAGICNUMBER 0x123456789abcdef

// umalloc_init
// Initialize the umalloc system.  This function should be called once
// before any other umalloc functions are called.
// __attribute__((constructor)) is used for gcc to call this function
// before main() is called.
__attribute__((constructor)) void umalloc_init(void) {
  // maybe do something here, maybe not

  return;
}

// umalloc
// Allocate a block of memory of the given size.  Returns a pointer to
// the allocated block of memory, or NULL if the allocation fails.
void *umalloc(size_t size) {
  /*
  use sbrk() to allocate memory from the heap.
  For simplicity, you can alloc a block of memory that is a power of 2,
  rather than the exact size requested.
  */

  return NULL;
}

// ufree
// Free a block of memory that was previously allocated by umalloc.
// If ptr is NULL, no operation is performed.
void ufree(void *ptr) {
  if (ptr == NULL) {
    return;
  }

  return;
}
