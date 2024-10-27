#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <unistd.h>

#define SHM_NAME "system_v_shm"
#define SHM_SIZE 1024

int main() {
  int shmid;
  char *shm;
  const char *message = "Hi!";

  key_t key = ftok(SHM_NAME, 54);

  shmid = shmget(key, SHM_SIZE, IPC_CREAT | 0666);
  if (shmid < 0) {
    perror("shmget");
    exit(EXIT_FAILURE);
  }

  shm = shmat(shmid, NULL, 0);
  if (shm == (char *)-1) {
    perror("shmat");
    exit(EXIT_FAILURE);
  }
  memcpy(shm, message, strlen(message) + 1);

  printf("Отправил сообщение: %s\nСервер ждет сообщения 10 секунд\n", message);
  sleep(10);
  printf("Сервер получил сообщение от клиента: %s\n", shm);

  if (shmdt(shm) == -1) {
    perror("shmdt");
    exit(EXIT_FAILURE);
  }

  if (shmctl(shmid, IPC_RMID, NULL) == -1) {
    perror("shmctl");
    exit(EXIT_FAILURE);
  }

  return 0;
}