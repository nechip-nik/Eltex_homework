#include <fcntl.h>
#include <semaphore.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define SHM_NAME "/chat_shm"
#define SEM_NAME "/chat_sem"
#define MAX_CLIENTS 10
#define MAX_MSG_LEN 256
#define MAX_MESSAGES 100

typedef struct {
  char name[20];
  int active;
} Client;

typedef struct {
  char message[MAX_MSG_LEN];
  char sender[20];
} Message;

typedef struct {
  Client clients[MAX_CLIENTS];
  Message messages[MAX_MESSAGES];
  int client_count;
  int message_count;
} SharedMemory;

SharedMemory *shm;
sem_t *sem;

void cleanup() {
  shm_unlink(SHM_NAME);
  sem_unlink(SEM_NAME);
  munmap(shm, sizeof(SharedMemory));
  sem_close(sem);
}

void sigint_handler(int sig) {
  cleanup();
  exit(0);
}

int main() {
  signal(SIGINT, sigint_handler);

  int shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
  if (shm_fd == -1) {
    perror("shm_open");
    exit(1);
  }

  if (ftruncate(shm_fd, sizeof(SharedMemory)) == -1) {
    perror("ftruncate");
    exit(1);
  }

  shm = mmap(NULL, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED,
             shm_fd, 0);
  if (shm == MAP_FAILED) {
    perror("mmap");
    exit(1);
  }

  sem = sem_open(SEM_NAME, O_CREAT, 0666, 1);
  if (sem == SEM_FAILED) {
    perror("sem_open");
    exit(1);
  }

  memset(shm, 0, sizeof(SharedMemory));

  while (1) {
    sem_wait(sem);

    for (int i = 0; i < shm->client_count; i++) {
      if (shm->clients[i].active) {
        // for (int j = 0; j < shm->message_count; j++) {
        //   printf("[%s]: %s\n", shm->messages[j].sender,
        //          shm->messages[j].message);
        // }
        shm->clients[i].active = 0;
      }
    }

    sem_post(sem);
    sleep(1);
  }

  cleanup();
  return 0;
}