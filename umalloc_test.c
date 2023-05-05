//! Tests for umalloc







//! NOT MODIFY ANYTHING IN THIS FILE





#include "umalloc.h"
#include <memory.h>
#include <stdio.h>
#include <stdlib.h>

void exit_test() {
  printf("Ops, something wrong with umalloc\n");
  printf("You don't pass all the test\n");
  exit(0);
}

int main(void) {
  printf("umalloc_test.c start\n");

  // allocate 8 bytes for 16 times
  for (int i = 0; i < 16; i++) {
    void *ptr = umalloc(8);
    if (ptr == NULL) {
      exit_test();
    }
    memset(ptr, i, 8);
    ufree(ptr);
  }

  // allocate 342 bytes for 8 times
  for (int i = 0; i < 8; i++) {
    void *ptr = umalloc(342);
    if (ptr == NULL) {
      exit_test();
    }
    memset(ptr, i, 342);
    ufree(ptr);
  }

  // allocate 1024 bytes for 4 times
  for (int i = 0; i < 4; i++) {
    void *ptr = umalloc(1024);
    if (ptr == NULL) {
      exit_test();
    }
    memset(ptr, i, 1024);
    ufree(ptr);
  }

  // allocate 1 to 100 bytes
  for (int i = 1; i <= 100; i++) {
    void *ptr = umalloc(i);
    if (ptr == NULL) {
      exit_test();
    }
    memset(ptr, i, i);
    ufree(ptr);
  }

  // pointer array of char[100]
  char *ptrs[100];
  for (int i = 0; i < 100; i++) {
    ptrs[i] = umalloc(100);
    if (ptrs[i] == NULL) {
      exit_test();
    }
    memset(ptrs[i], i, 100);
  }

  // allocate 100 pointers of char[100],
  // then randomly ufree them
  char *parr[100];
  for (int i = 0; i < 100; i++) {
    parr[i] = umalloc(100);
    if (parr[i] == NULL) {
      exit_test();
    }
    memset(parr[i], i, 100);
  }

  // ufree them randomly
  for (int i = 0; i < 100; i++) {
    int idx = rand() % 100;
    ufree(parr[idx]);
    parr[idx] = NULL;
  }

  // ufree rest of the pointers
  for (int i = 0; i < 100; i++) {
    if (parr[i] != NULL) {
      ufree(parr[i]);
    }
  }

  // ufree all the pointers
  for (int i = 0; i < 100; i++) {
    // check if the content is correct
    for (int j = 0; j < 100; j++) {
      if (ptrs[i][j] != i) {
        exit_test();
      }
    }
    ufree(ptrs[i]);
  }

  printf("umalloc works as expected!\n");
  printf("You pass all the tests!\n");

  return 0;
}
