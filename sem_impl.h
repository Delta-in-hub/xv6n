#pragma once

#include <pthread.h>

struct sem {
  // sem implementation



};

int sem_init(struct sem *, int);
int sem_p(struct sem *);
int sem_v(struct sem *);
