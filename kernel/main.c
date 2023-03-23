#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "riscv.h"
#include "defs.h"

volatile static int started = 0;

// start() jumps here in supervisor mode on all CPUs.
int main() {
  if (cpuid() == 0) {
    consoleinit();
    printfkinit();
    printfk("\n");
    printfk("xv6 kernel is booting\n");
    printfk("\n");
    kinit();              // physical page allocator
    kvminit();            // create kernel page table
    kvminithart();        // turn on paging
    procinit();           // process table
    trapinit();           // trap vectors
    trapinithart();       // install kernel trap vector
    plicinit();           // set up interrupt controller
    plicinithart();       // ask PLIC for device interrupts
    binit();              // buffer cache
    iinit();              // inode table
    fileinit();           // file table
    virtio_disk_init();   // emulated hard disk
    userinit();           // first user process
    __sync_synchronize(); // It is a atomic builtin for full memory barrier.
    // No memory operand will be moved across the operation, either forward
    // or backward. Further, instructions will be issued as necessary to
    // prevent the processor from speculating loads across the operation and
    // from queuing stores after the operation.
    started = 1;
  } else {
    while (started == 0)
      ;
    __sync_synchronize();
    printfk("hart %d starting\n", cpuid());
    kvminithart();  // turn on paging
    trapinithart(); // install kernel trap vector
    plicinithart(); // ask PLIC for device interrupts
  }

  scheduler();
}
