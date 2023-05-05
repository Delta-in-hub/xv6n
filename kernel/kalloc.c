// Physical memory allocator, for user processes,
// kernel stacks, page-table pages,
// and pipe buffers. Allocates whole 4096-byte pages.

#include "types.h"
#include "param.h"
#include "memlayout.h"
#include "spinlock.h"
#include "riscv.h"
#include "defs.h"
#include "list.h"

extern char end[]; // first address after kernel.
                   // defined by kernel.ld.

struct free_chunk {
  void *start;         // start address of free chunk
  size_t size_in_page; // size of free chunk in page
  list_entry entry;    // list entry
};

static void free_chunk_init(struct free_chunk *chunk, void *start,
                            size_t size_in_page);
static void free_chunk_free(list_entry *chunk_list, void *start, size_t npages);
static void *free_chunk_alloc(list_entry *chunk_list, size_t npages);

struct {
  struct spinlock lock;
  list_entry free_chunk_list;
} kmem;

void kinit() {
  initlock(&kmem.lock, "kmem");

  uint64 memstart = PGROUNDUP((uint64)end);
  uint64 memend = PGROUNDDOWN((uint64)PHYSTOP);
  size_t totalpages = (memend - memstart) / PGSIZE;

  struct free_chunk *chunk = (struct free_chunk *)memstart;
  free_chunk_init(chunk, (void *)memstart, totalpages);

  pushBack(&kmem.free_chunk_list, &chunk->entry);
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

// Physical Memory Manager, unit is page
//! First fit Now
//! You need to modify code to the best fit algorithm

/**
 * @brief Initialize a free chunk
 * @param chunk the chunk to be initialized
 * @param start start address of the chunk
 * @param size_in_page size of the chunk in page
 */
static void free_chunk_init(struct free_chunk *chunk, void *start,
                            size_t size_in_page) {
  chunk->start = start;
  chunk->size_in_page = size_in_page;
  list_entry_init(&chunk->entry);
}

/**
 * @brief allocate npages pages of physical memory from chunk_list ,
 * must be called with lock held
 * @param chunk_list the list of free chunks
 * @param npages number of pages to allocate
 * @return void* , the start address of allocated memory, NULL if failed
 */
static void *free_chunk_alloc(list_entry *chunk_list, size_t npages) {

  traverse(p, chunk_list) {
    struct free_chunk *current_chunk = listEntry(p, struct free_chunk, entry);

    // First fit now
    if (current_chunk->size_in_page >=
        npages) { // There is enough space in this chunk

      char *ret = current_chunk->start; // start address of allocated memory

      size_t remain_pages = current_chunk->size_in_page - npages;

      // If there is remain space in this chunk, we need to update the chunk
      struct free_chunk *next_chunk =
          (struct free_chunk *)(current_chunk->start + npages * PGSIZE);

      if (remain_pages == 0) { // If there is no remain space, just delete this
                               // chunk from the free chunk list
        delEntry(p);
      } else {
        // (chunk->start) not always equal to (chunk)
        // If (chunk->start) less than (chunk), such as:
        //  | -page0- | -page1- | -chunk struct- | -page3- |
        //! ^chunk->start            ^chunk
        // and then allocate one page, we just need to update the chunk->start
        // and chunk->size_in_page Else, such as:
        //  | -page0- | -chunk struct- | -page2- | -page3- |
        //! ^chunk->start   ^chunk
        // then allocate two pages, we need to init a new chunk struct on the
        // page2, and push it back to the free chunk list

        if (current_chunk < next_chunk) {
          free_chunk_init(next_chunk, next_chunk, remain_pages);
          pushBack(&next_chunk->entry, p);
          delEntry(p);
        } else {
          current_chunk->start = next_chunk;
          current_chunk->size_in_page = remain_pages;
        }
      }
      return ret;
    }
  }

  return NULL;
}

/**
 * @brief try to merge two free chunks, must be called with lock held,
 * merge chunk1 to chunk0
 * @param chunk0
 * @param chunk1
 * @return 1 if merged, 0 if not
 */
static int try_to_merge_two_free_chunk(struct free_chunk *chunk0,
                                       struct free_chunk *chunk1) {

  if (chunk0->start + chunk0->size_in_page * PGSIZE == chunk1->start) {
    chunk0->size_in_page += chunk1->size_in_page;
    return 1;
  } else if (chunk1->start + chunk1->size_in_page * PGSIZE == chunk0->start) {
    chunk0->start = chunk1->start;
    chunk0->size_in_page += chunk1->size_in_page;
    return 1;
  }

  return 0;
}

/**
 * @brief free npages pages of physical memory, give it back to chunk_list,
 * must be called with lock held
 * @param chunk_list the list of free chunks
 * @param start address of the memory to be freed
 * @param npages number of pages to free
 */
static void free_chunk_free(list_entry *chunk_list, void *start,
                            size_t npages) {
  struct free_chunk *chunk = (struct free_chunk *)start;

  free_chunk_init(chunk, start, npages);

  int insert_flag = 0;

  traverse(p, chunk_list) {
    struct free_chunk *pchunk = listEntry(p, struct free_chunk, entry);

    if (!insert_flag) {
      // try to merge reback chunk to the current chunk
      insert_flag = try_to_merge_two_free_chunk(pchunk, chunk);
    }

    if (!isLast(p, chunk_list)) {
      // p is not the last chunk,thus we can try to merge current chunk to the
      // next chunk
      struct free_chunk *nchunk = listEntry(p->succ, struct free_chunk, entry);

      // merge to the next chunk and delete the current chunk
      int res = try_to_merge_two_free_chunk(nchunk, pchunk);

      if (res) // if merged, delete the current chunk
        delEntry(p);
    }
  }

  if (!insert_flag) // if reback chunk is not merged with any chunk, insert it
                    // to the free chunk list
    pushFront(&chunk->entry, chunk_list);
}

// Allocate n pages of physical memory.
void *kallocn(size_t npages) {
  if (npages == 0)
    return NULL;
  void *r = NULL;
  acquire(&kmem.lock);
  r = free_chunk_alloc(&kmem.free_chunk_list, npages);
  release(&kmem.lock);
  return r;
}

// Free n pages of physical memory pointed at pa.
void kfreen(void *pa, size_t npages) {
  if (!pa)
    return;
  acquire(&kmem.lock);
  free_chunk_free(&kmem.free_chunk_list, pa, npages);
  release(&kmem.lock);
}
