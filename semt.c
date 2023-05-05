#include <pthread.h>
#include <semaphore.h>
#include <stdio.h>

//  桌子上有一个空盘子，允许存放一只水果。
//  爸爸（生产者 1）可以向盘中放苹果
//  妈妈（生产者 2）向盘子中放橘子
//  女儿（消费者 1）专门吃盘子中的苹果
//  儿子（消费者 2）专门吃盘子中的橘子。

#define N 4

// sem_t plate, apple, orange;
sem_t _maybe_something_just_for_hint_;

void p_dad(void) {
  int cnt = 0;
  while (cnt < N) {

    printf("Dad put an apple on the plate.\n");

    cnt++;
  }
}
void p_mom(void) {
  int cnt = 0;
  while (cnt < N) {

    printf("Mom put an orange on the plate.\n");

    cnt++;
  }
}
void c_daughter(void) {
  int cnt = 0;
  while (cnt < N) {

    printf("Daughter eat an apple, which is the %dth apple.\n", cnt + 1);

    cnt++;
  }
}
void c_son(void) {
  int cnt = 0;
  while (cnt < N) {

    printf("Son eat an orange, which is the %dth orange.\n", cnt + 1);

    cnt++;
  }
}

int main(void) {
  setbuf(stdout, NULL);

  pthread_t p1, p2, c1, c2;
  pthread_create(&p1, NULL, (void *)p_dad, NULL);




  pthread_join(p1, NULL);




  return 0;
}
