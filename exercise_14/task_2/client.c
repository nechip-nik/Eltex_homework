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
WINDOW *chat_win, *input_win, *users_win;

void *receive_messages(void *arg) {
  while (1) {
    sem_wait(sem);

    // Очищаем только содержимое окна chat_win
    wclear(chat_win);
    int max_y, max_x;
    getmaxyx(chat_win, max_y, max_x);
    int start_index =
        shm->message_count > max_y - 2 ? shm->message_count - (max_y - 2) : 0;
    for (int i = start_index; i < shm->message_count; i++) {
      mvwprintw(chat_win, i - start_index, 0, "[%s]: %s",
                shm->messages[i].sender, shm->messages[i].message);
    }
    wrefresh(chat_win);

    // Очищаем только содержимое окна users_win
    wclear(users_win);
    for (int i = 0; i < shm->client_count; i++) {
      mvwprintw(users_win, i + 1, 0, "%s", shm->clients[i].name);
    }
    wrefresh(users_win);

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

  int max_y, max_x;
  getmaxyx(stdscr, max_y, max_x);

  chat_win = newwin(max_y - 3, max_x - 20, 0, 0);
  input_win = newwin(3, max_x, max_y - 3, 0);
  users_win = newwin(max_y - 3, 20, 0, max_x - 20);

  // Включаем прокрутку для окна чата
  scrollok(chat_win, TRUE);

  refresh();
  wrefresh(chat_win);
  wrefresh(input_win);
  wrefresh(users_win);

  refresh();
  getnstr(client_name, 19);

  sem_wait(sem);
  int client_index = shm->client_count;
  if (client_index < MAX_CLIENTS) {
    strncpy(shm->clients[client_index].name, client_name, 20);
    shm->clients[client_index].active = 1;
    shm->client_count++;
  }
  sem_post(sem);

  pthread_t thread;
  pthread_create(&thread, NULL, receive_messages, NULL);

  char msg[MAX_MSG_LEN];
  while (1) {
    wmove(input_win, 1, 0);
    wprintw(input_win, "%s: ", client_name);
    wrefresh(input_win);
    wgetnstr(input_win, msg, MAX_MSG_LEN - 1);
    send_message(msg);
  }

  endwin();
  munmap(shm, sizeof(SharedMemory));
  sem_close(sem);
  return 0;
}