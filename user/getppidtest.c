// getppid test file

//! NOT MODIFY ANYTHING IN THIS FILE

#include "kernel/types.h"
#include "user/user.h"

int main(int argc, char *argv[]) {

  int pid, ppid;

  int childpid = fork();
  if (childpid == 0) {
    pid = getpid();
    ppid = getppid();
    printf("pid: %d, ppid: %d\n", pid, ppid);
    if (ppid >= pid)
      printf("ERROR: ppid >= pid\n");
    exit(0);
  }
  wait(0);

  pid = getpid();
  ppid = getppid();
  printf("pid: %d, ppid: %d\n", pid, ppid);
  // pid: 3, ppid: 2

  if (pid > ppid)
    printf("getppidtest PASS\n");
  exit(0);
}
