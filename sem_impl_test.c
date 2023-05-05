

// ! TEST FILE FOR sem_impl





// ! NOT MODIFY THIS FILE






#include "sem_impl.h"
#include <stdio.h>
#include <pthread.h>

#define N 4

struct sem plate, apple, orange;

void p_dad(void) {
  int cnt = 0;
  while (cnt < N) {
    sem_p(&plate);
    printf("Dad put an apple on the plate.\n");
    sem_v(&apple);
    cnt++;
  }
}
void p_mom(void) {
  int cnt = 0;
  while (cnt < N) {
    sem_p(&plate);
    printf("Mom put an orange on the plate.\n");
    sem_v(&orange);
    cnt++;
  }
}
void c_daughter(void) {
  int cnt = 0;
  while (cnt < N) {
    sem_p(&apple);
    printf("Daughter eat an apple, which is the %dth apple.\n", cnt + 1);
    sem_v(&plate);
    cnt++;
  }
}
void c_son(void) {
  int cnt = 0;
  while (cnt < N) {
    sem_p(&orange);
    printf("Son eat an orange, which is the %dth orange.\n", cnt + 1);
    sem_v(&plate);
    cnt++;
  }
}

int main(void) {
  setbuf(stdout, NULL);

  sem_init(&plate, 1);
  sem_init(&apple, 0);
  sem_init(&orange, 0);

  pthread_t p1, p2, c1, c2;
  pthread_create(&p1, NULL, (void *)p_dad, NULL);
  pthread_create(&p2, NULL, (void *)p_mom, NULL);
  pthread_create(&c1, NULL, (void *)c_daughter, NULL);
  pthread_create(&c2, NULL, (void *)c_son, NULL);
  pthread_join(p1, NULL);
  pthread_join(p2, NULL);
  pthread_join(c1, NULL);
  pthread_join(c2, NULL);
  return 0;
}
