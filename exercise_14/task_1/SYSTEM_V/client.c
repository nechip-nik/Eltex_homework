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
  const char *message = "Hello!";
  key_t key = ftok(SHM_NAME, 54);

  shmid = shmget(key, SHM_SIZE, 0666);
  if (shmid < 0) {
    perror("shmget");
    exit(EXIT_FAILURE);
  }

  shm = shmat(shmid, NULL, 0);
  if (shm == (char *)-1) {
    perror("shmat");
    exit(EXIT_FAILURE);
  }

  printf("Сообщение полученно от сервера: %s\n", shm);
  memcpy(shm, message, strlen(message) + 1);
  printf("Сообщение отправленно на сервер: %s\n", message);

  if (shmdt(shm) == -1) {
    perror("shmdt");
    exit(EXIT_FAILURE);
  }

  return 0;
}