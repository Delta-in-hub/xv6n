#include "kernel/types.h"
#include "user/user.h"

int sleep1() {
  int n = 1e6;
  int *arr = (int *)malloc(n * sizeof(int));

  for (int i = 0; i < n; i++) {
    arr[i] += i;
  }

  int sum = 0;
  for (int i = 0; i < n; i++) {
    sum += arr[i];
  }

  free(arr);
  return sum;
}

int main(void) {

  int pid = fork();
  if (pid == 0) {
    // child
    sleep1();
    struct schedtime st;
    getschedtime(&st);
    printf("child last sched start at %d, end at %d\n", st.stime, st.etime);
    exit(0);
  } else {
    // parent
    wait(0);
    sleep1();
    struct schedtime st;
    getschedtime(&st);
    printf("parent last sched start at %d, end at %d\n", st.stime, st.etime);
  }

  exit(0);
}
