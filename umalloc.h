#ifndef __UMALLOC__
#define __UMALLOC__

void umalloc_init();

void *umalloc(unsigned long size);

void ufree(void *ptr);

#endif
