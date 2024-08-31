#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>

#define NUM_THREAD 4
int a = 0;
void *increment_test() {
  for (size_t i = 0; i < 40000000; i++) {
    a = a + 1;
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