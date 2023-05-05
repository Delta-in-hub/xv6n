
// TEST FILE FOR EXCEPTION HANDLING

// ! NOT MODIFY THIS FILE

#include "kernel/types.h"
#include "kernel/stat.h"
#include "kernel/fcntl.h"
#include "user/user.h"

#define N 3

int main(void) {
  char *c = 0;
  for (; c < (char *)N; c++) {
    char p = *c;
    *c = p * p;
  }

  c = (char *)-1;
  for (; c > (char *)(-1 - N); c--) {
    char p = *c;
    *c = p * p;
  }

  return 0;
}
