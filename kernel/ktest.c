#include "types.h"
#include "param.h"
#include "riscv.h"
#include "spinlock.h"
#include "defs.h"

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

  void *p = kallocn(32 * 1024 * 1024);
  if (p)
    panic("kalloc_test: kallocn should fail");

  return 1;
}
