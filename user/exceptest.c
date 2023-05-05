
// TEST FILE FOR EXCEPTION HANDLING

// ! NOT MODIFY THIS FILE

#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

#define N 3

int main(void) {

  char arr[32];

  char *c = arr;
  for (; c < (char *)N; c++) {
    char p = *c;
    *c = p * p;
  }

  asm volatile("ebreak");

  c = (char *)arr + 31;
  for (; c > (char *)(arr + 1); c--) {
    char p = *c;
    *c = p * p;
  }

  return 0;
}
