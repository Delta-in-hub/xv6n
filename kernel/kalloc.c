// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"
#include "kmath.h"
#include "list.h"

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

static void *buddy_alloc(size_t npages);
static void buddy_free(void *pa, size_t npages);

/*
buddy tree be like (number is idx) :
                          1
                2                       3
          4           5           6           7
      8       9   10      11  12      13  14      15
*/

struct buddyblock {
  void *pa;       // physical address of this block
  size_t idx;     // index of this block in the buddy tree
  list_entry ent; // list entry of this block
};

struct {
  struct spinlock lock;
  list_entry list[RAMNPAGES_LOG2 + 1];
  /*
  list[0] : buddyblock list head with size (2^0 = 1) pages
  list[1] : buddyblock list head with size (2^1 = 2) pages
  list[2] : buddyblock list head with size (2^2 = 4) pages
  ...
  list[MAXNLOG2] : buddyblock list head with size (2^MAXNLOG2) pages

  We call i of list[i] as level of this list.
  */

  uint MAXNLOG2; // 2^MAXNLOG2 is the max block size in pages
                 // MAXNLOG2 <= RAMNPAGES_LOG2, because buddy tree structure
                 // needs memory too. A part of memory is used for buddy tree
                 // itself.

} kmem;

#define LCHIND(x) ((x)*2)
#define RCHIND(x) ((x)*2 + 1)
#define PARENT(x) ((x) / 2)
#define BUDDY(x) ((x) % 2 == 0 ? (x) + 1 : (x)-1)

// Convert block index to number of pages
static inline uint idx2npages(uint idx) {
  // idx : page number size of this block
  // 1 : kmem.MAXNLOG2
  // 2,3 : kmem.MAXNLOG2/2
  // 4,5,6,7 : kmem.MAXNLOG2/4
  uint n = rounddown2i(idx);
  return (1 << (kmem.MAXNLOG2)) / n;
}

// Convert (address, number of pages) to block index
static inline uint npages2idx(void *pa, uint npages) {

  uint n = log2i(npages);

  uint startidx = 1 << (kmem.MAXNLOG2 - n); // the very first block's index of
                                            // this page size

  uint64 memstart = PGROUNDUP((uint64)end);
  uint64 offset = (uint64)pa - memstart;
  uint offsetidx = offset / (PGSIZE * npages); // distance from the very first
                                               // block

  return startidx + offsetidx;
}

void kinit() {
  initlock(&kmem.lock, "kmem");

  uint64 memstart = PGROUNDUP((uint64)end);
  uint64 memend = PGROUNDDOWN((uint64)PHYSTOP);
  uint64 totalpages = (memend - memstart) / PGSIZE;
  totalpages = rounddown2i(totalpages); // buddy system only support 2^n pages

  uint64 maxnlog2 = log2i(totalpages);
  kmem.MAXNLOG2 = maxnlog2;
  if (maxnlog2 > RAMNPAGES_LOG2)
    panic("kinit: too much memory");

  for (int i = 0; i <= maxnlog2; i++) {
    list_entry_init(&kmem.list[i]);
  }

  struct buddyblock *block = (struct buddyblock *)memstart;
  block->pa = (void *)memstart;
  block->idx = 1; // idx start from 1
  list_entry_init(&block->ent);

  // In init, we have only one block with size 2^maxnlog2, which is the whole
  // memory available. So we insert it to the list of 2^maxnlog2
  pushFront(&kmem.list[maxnlog2], &block->ent);
}

// Free the page of physical memory pointed at by pa,
// which normally should have been returned by a
// call to kalloc().  (The exception is when
// initializing the allocator; see kinit above.)
void kfree(void *pa) {
  if (((uint64)pa % PGSIZE) != 0 || (char *)pa < end || (uint64)pa >= PHYSTOP)
    panic("kfree");

  // Fill with junk to catch dangling refs.
  memset(pa, 1, PGSIZE);

  kfreen(pa, 1);
}

// Allocate one 4096-byte page of physical memory.
// Returns a pointer that the kernel can use.
// Returns 0 if the memory cannot be allocated.
void *kalloc(void) {
  void *r = kallocn(1);

  if (r)
    memset((char *)r, 5, PGSIZE); // fill with junk
  return r;
}

void *kallocn(size_t npages) {
  if (npages == 0)
    return NULL;
  void *r = NULL;
  acquire(&kmem.lock);
  r = buddy_alloc(npages);
  release(&kmem.lock);
  return r;
}

void kfreen(void *pa, size_t npages) {
  if (!pa)
    return;
  acquire(&kmem.lock);
  buddy_free(pa, npages);
  release(&kmem.lock);
}

// Buddy Memory Management
// https://en.wikipedia.org/wiki/Buddy_memory_allocation

// split entp(bigger buddy block) into two small blocks,
// and insert to the (level - 1) buddy block list
static void buddy_split(list_entry *entp, int level) {
  struct buddyblock *block = listEntry(entp, struct buddyblock, ent);


  if (level == 0)
    panic("split_to_nextlevel: level == 0");


}

// merge two blocks into a big block,
// and insert to the (level + 1) list
static list_entry *buddy_merge(list_entry *b1, list_entry *b2) {
  struct buddyblock *block1 = listEntry(b1, struct buddyblock, ent);
  struct buddyblock *block2 = listEntry(b2, struct buddyblock, ent);
  if (block1->idx != BUDDY(block2->idx))
    panic("buddy_merge: block1->idx != BUDDY(block2->idx)");





}

// allocate a block with npages, must be called with lock held
static void *buddy_alloc(size_t npages) {
  npages = roundup2i(npages);
  uint targetn = log2i(npages);
  if (targetn > kmem.MAXNLOG2)
    return NULL;
  if (!isEmpty(&kmem.list[targetn])) {
    list_entry *entp = popFront(&kmem.list[targetn]);
    if (!entp)
      panic("buddy_alloc: popFront failed return NULL");
    struct buddyblock *block = listEntry(entp, struct buddyblock, ent);
    return block->pa;
  }













  return NULL;
}

static void buddy_free(void *pa, size_t npages) {
  if (!pa)
    return;
  npages = roundup2i(npages);
  uint idx = npages2idx(pa, npages);
  if (idx == 0)
    panic("buddy_free: idx == 0");




}
