#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define NUM_MUTEXES 5
#define NUM_BUYERS 3

pthread_mutex_t mutexes[NUM_MUTEXES];
int goods[NUM_MUTEXES];

typedef struct {
  int id;
  int need;
} BuyerArgs;

void *buyer(void *arg) {
  BuyerArgs *args = (BuyerArgs *)arg;
  int id = args->id;
  int need = args->need;
  int buff;
  while (need > 0) {
    for (int i = 0; i < NUM_MUTEXES; i++) {
      if (pthread_mutex_trylock(&mutexes[i]) == 0) {
        if (goods[i] > 0) {
          printf("Покупатель %d зашел в магазин %d с потребностью %d\n", id,
                 i + 1, need);
          buff = (need < goods[i]) ? need : goods[i];
          goods[i] -= buff;
          need -= buff;
          printf(
              "Покупатель %d забирает %d товаров из магазина %d, осталось %d\n",
              id, buff, i + 1, goods[i]);
        }
        pthread_mutex_unlock(&mutexes[i]);
        printf("Покупатель %d уснул с потребностью %d\n", id, need);
        sleep(2);
      }
      if (need <= 0) {
        break;
      }
    }
  }
  printf("---------------------Покупатель %d удовлетворил свои "
         "потребности----------------------\n",
         id);
  pthread_exit(NULL);
}

void *loader() {
  while (1) {
    for (int i = 0; i < NUM_MUTEXES; i++) {
      if (pthread_mutex_trylock(&mutexes[i]) == 0) {
        printf("Загрузчик зашел в магазин %d\n", i + 1);
        goods[i] += 5000;
        printf("Загрузчик добавил 5000 товаров в магазин %d, теперь там %d "
               "товаров\n",
               i + 1, goods[i]);
        pthread_mutex_unlock(&mutexes[i]);
      }
      sleep(1);
    }
    printf("Загрузчик уснул\n");
  }
  pthread_exit(NULL);
}

int main() {
  pthread_t threads[NUM_BUYERS + 1];
  BuyerArgs buyer_args[NUM_BUYERS];
  srand(time(NULL));

  for (int i = 0; i < NUM_MUTEXES; i++) {
    goods[i] = 9500 + rand() % 1001;
  }

  for (int i = 0; i < NUM_BUYERS; i++) {
    buyer_args[i].id = i + 1;
    buyer_args[i].need = 95000 + rand() % 10001;
  }

  for (int i = 0; i < NUM_MUTEXES; ++i) {
    pthread_mutex_init(&mutexes[i], NULL);
  }

  for (int i = 0; i < NUM_BUYERS; i++) {
    if (pthread_create(&threads[i], NULL, buyer, &buyer_args[i]) != 0) {
      printf("Ошибка при создании потока покупателя %d\n", i + 1);
      exit(-1);
    }
  }

  if (pthread_create(&threads[NUM_BUYERS], NULL, loader, NULL) != 0) {
    printf("Ошибка при создании потока загрузчика\n");
    exit(-1);
  }

  for (int i = 0; i < NUM_BUYERS; i++) {
    pthread_join(threads[i], NULL);
  }

  pthread_cancel(threads[NUM_BUYERS]);
  pthread_join(threads[NUM_BUYERS], NULL);

  printf("Все потоки успешно завершили работу\n");

  for (int i = 0; i < NUM_MUTEXES; ++i) {
    pthread_mutex_destroy(&mutexes[i]);
  }

  pthread_exit(NULL);
  return 0;
}