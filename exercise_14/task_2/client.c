#include <fcntl.h>
#include <ncurses.h>
#include <pthread.h>
#include <semaphore.h>
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
char client_name[20];

void *receive_messages(void *arg) {
  while (1) {
    sem_wait(sem);

    for (int i = 0; i < shm->message_count; i++) {
      mvprintw(i, 0, "[%s]: %s", shm->messages[i].sender,
               shm->messages[i].message);
    }

    refresh();
    sem_post(sem);
    sleep(1);
  }
  return NULL;
}

void send_message(const char *msg) {
  sem_wait(sem);

  if (shm->message_count < MAX_MESSAGES) {
    strncpy(shm->messages[shm->message_count].message, msg, MAX_MSG_LEN);
    strncpy(shm->messages[shm->message_count].sender, client_name, 20);
    shm->message_count++;
  }

  sem_post(sem);
}

int main() {
  int shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
  if (shm_fd == -1) {
    perror("shm_open");
    exit(1);
  }

  shm = mmap(NULL, sizeof(SharedMemory), PROT_READ | PROT_WRITE, MAP_SHARED,
             shm_fd, 0);
  if (shm == MAP_FAILED) {
    perror("mmap");
    exit(1);
  }

  sem = sem_open(SEM_NAME, 0);
  if (sem == SEM_FAILED) {
    perror("sem_open");
    exit(1);
  }

  initscr();
  cbreak();
  noecho();
  scrollok(stdscr, TRUE);

  printw("Enter your name: ");
  refresh();
  getnstr(client_name, 19);

  sem_wait(sem);
  for (int i = 0; i < MAX_CLIENTS; i++) {
    if (shm->clients[i].active == 0) {
      strncpy(shm->clients[i].name, client_name, 20);
      shm->clients[i].active = 1;
      shm->client_count++;
      break;
    }
  }
  sem_post(sem);

  pthread_t thread;
  pthread_create(&thread, NULL, receive_messages, NULL);

  char msg[MAX_MSG_LEN];
  while (1) {
    mvprintw(shm->message_count, 0, "%s: ", client_name);
    refresh();
    getnstr(msg, MAX_MSG_LEN - 1);
    send_message(msg);
  }

  endwin();
  munmap(shm, sizeof(SharedMemory));
  sem_close(sem);
  return 0;
}