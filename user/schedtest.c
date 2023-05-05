#include "kernel/types.h"
#include "kernel/fcntl.h"
#include "kernel/stat.h"
#include "user/user.h"

// fibnaaci
int fib(int n) {
  if (n == 0) {
    return 0;
  }
  if (n == 1) {
    return 1;
  }
  return fib(n - 1) + fib(n - 2);
}

#define N 35

int main(void) {

  int pid = fork();
  if (pid == 0) {
    printf("fib == %d\n", fib(N));
  }
  pid = fork();
  if (pid == 0) {
    printf("fib == %d\n", fib(N));
  }
  pid = fork();
  if (pid == 0) {
    printf("fib == %d\n", fib(N));
  }
  wait(0);
  printf("fib == %d\n", fib(N));
  exit(0);
}
