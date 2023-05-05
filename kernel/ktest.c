#include "types.h"
#include "param.h"
#include "riscv.h"
#include "spinlock.h"
#include "defs.h"
#include <stddef.h>

static int kalloc_test();

int ktest() {
  int flag = 1;
  flag &= kalloc_test();

  if (flag) {
    printfk("ktest: cpu%d pass all tests\n", cpuid());
  } else {
    printfk("ktest: cpu%d not pass all tests\n", cpuid());
    panic("ktest: failed\n");
  }

  return flag;
}

static int kalloc_test() {

  void **arr = kallocn(1);
  if (arr == 0)
    panic("kalloc_test: kallocn failed");

  for (uint i = 1; i <= 32; i++) {
    arr[i] = kallocn(3);
    if (arr[i] == 0)
      panic("kalloc_test: kallocn failed");
    memset(arr[i], i, 3 * PGSIZE);
  }

  for (uint i = 1; i <= 32; i++) {
    void *p = kallocn(i);
    if (p)
      memset(p, i, i * PGSIZE);
    char *ch = p;
    for (uint j = 0; j < i * PGSIZE; j++) {
      if (ch[j] != i)
        panic("kalloc_test: read after write failed");
    }
    kfreen(p, i);
  }

  for (uint i = 1; i <= 32; i++) {
    void *p = kallocn(1);
    if (!p)
      panic("kalloc_test: kallocn failed");
    else
      memset(p, i, PGSIZE);
    char *ch = p;
    for (uint j = 0; j < PGSIZE; j++) {
      if (ch[j] != i)
        panic("kalloc_test: read after write failed");
    }
    kfreen(p, 1);
  }

  for (uint i = 28; i >= 1; i--) {
    char *ch = arr[i];
    for (uint j = 0; j < 3 * PGSIZE; j++) {
      if (ch[j] != i)
        panic("kalloc_test: read after write failed");
    }
    kfreen(arr[i], 3);
  }

  kfreen(arr[30], 3);
  kfreen(arr[32], 3);
  kfreen(arr[29], 3);
  kfreen(arr[31], 3);

  kfreen(arr, 1);

  void *p = kallocn(32 * 1024 * 1024);
  if (p)
    panic("kalloc_test: kallocn should fail");

  return 1;
}
