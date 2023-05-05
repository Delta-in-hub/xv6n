#include <pthread.h>
#include <stdio.h>


//! 请编写，完善以下程序

//  桌子上有一个空盘子，允许存放一只水果。
//  爸爸（生产者 1）可以向盘中放苹果
//  妈妈（生产者 2）向盘子中放橘子
//  女儿（消费者 1）专门吃盘子中的苹果
//  儿子（消费者 2）专门吃盘子中的橘子。

#define N 4

enum Fruit { None = 0, APPLE, ORANGE };

struct Plate {
  enum Fruit fruit;
  int is_full;
} plate;

pthread_mutex_t mutex;
pthread_cond_t cond;

void p_dad(void) {
  int cnt = 0;
  while (cnt < N) {
    pthread_mutex_lock(&mutex);

    printf("Dad put an apple on the plate.\n");

    pthread_mutex_unlock(&mutex);
    cnt++;
  }
}
void p_mom(void) {
  int cnt = 0;
  while (cnt < N) {



    cnt++;
  }
}
void c_daughter(void) {
  int cnt = 0;
  while (cnt < N) {



    cnt++;
  }
}
void c_son(void) {
  int cnt = 0;
  while (cnt < N) {



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
