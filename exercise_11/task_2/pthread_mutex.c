#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREAD 4
int a = 0;
pthread_mutex_t mt1;
void *increment_test() {
  for (size_t i = 0; i < 10000000; i++) {
    pthread_mutex_lock(&mt1);
    a = a + 1;
    pthread_mutex_unlock(&mt1);
  }
  pthread_exit(NULL);
}
int main() {

  pthread_t threads[NUM_THREAD];
  int rc;
  long index;
  for (index = 0; index < NUM_THREAD; index++) {
    rc = pthread_create(&threads[index], NULL, increment_test, NULL);
    if (rc) {
      printf("Ошибка при создании потока %d\n", rc);
      exit(-1);
    }
  }
  for (index = 0; index < NUM_THREAD; index++) {
    pthread_join(threads[index], NULL);
  }
  printf("a = %d\n", a);
  pthread_exit(NULL);
}