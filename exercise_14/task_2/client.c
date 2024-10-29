#include <fcntl.h>
#include <semaphore.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define SHM_NAME "/chat_shm"
#define SEM_NAME "/chat_sem"
#define SHM_SIZE 1024
#define MAX_CLIENTS 10
#define MAX_MSG_LEN 256

typedef struct {
  char name[20];
  char message[MAX_MSG_LEN];
} ChatMessage;

typedef struct {
  int client_count;
  ChatMessage messages[MAX_CLIENTS];
} ChatRoom;

int main() {
  int shm_fd;
  ChatRoom *chat_room;
  sem_t *sem;
  char name[20];
  char message[MAX_MSG_LEN];

  // Открываем существующий сегмент разделяемой памяти
  shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
  if (shm_fd == -1) {
    perror("shm_open");
    exit(EXIT_FAILURE);
  }

  // Отображаем сегмент в память
  chat_room = (ChatRoom *)mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
                               shm_fd, 0);
  if (chat_room == MAP_FAILED) {
    perror("mmap");
    exit(EXIT_FAILURE);
  }

  // Открываем семафор
  sem = sem_open(SEM_NAME, 0);
  if (sem == SEM_FAILED) {
    perror("sem_open");
    exit(EXIT_FAILURE);
  }

  // Вводим имя клиента
  printf("Enter your name: ");
  fgets(name, sizeof(name), stdin);
  name[strcspn(name, "\n")] = 0; // Удаляем символ новой строки

  // Регистрируем клиента в чат-комнате
  sem_wait(sem);
  if (chat_room->client_count < MAX_CLIENTS) {
    strcpy(chat_room->messages[chat_room->client_count].name, name);
    chat_room->client_count++;
    printf("Client: Registered as %s\n", name);
  } else {
    printf("Client: Chat room is full.\n");
    exit(EXIT_FAILURE);
  }
  sem_post(sem);

  // Получаем все сообщения в комнате
  sem_wait(sem);
  for (int i = 0; i < chat_room->client_count; i++) {
    if (chat_room->messages[i].message[0] != '\0') {
      printf("%s: %s\n", chat_room->messages[i].name,
             chat_room->messages[i].message);
    }
  }
  sem_post(sem);

  // Отправляем сообщения в общий чат
  while (1) {
    printf("Enter message: ");
    fgets(message, sizeof(message), stdin);
    message[strcspn(message, "\n")] = 0; // Удаляем символ новой строки

    sem_wait(sem);
    for (int i = 0; i < chat_room->client_count; i++) {
      if (strcmp(chat_room->messages[i].name, name) == 0) {
        strcpy(chat_room->messages[i].message, message);
        break;
      }
    }
    sem_post(sem);

    // Выводим все сообщения в комнате
    sem_wait(sem);
    for (int i = 0; i < chat_room->client_count; i++) {
      if (chat_room->messages[i].message[0] != '\0') {
        printf("%s: %s\n", chat_room->messages[i].name,
               chat_room->messages[i].message);
        chat_room->messages[i].message[0] = '\0'; // Очищаем сообщение
      }
    }
    sem_post(sem);
  }

  // Удаляем отображение памяти
  if (munmap(chat_room, SHM_SIZE) == -1) {
    perror("munmap");
    exit(EXIT_FAILURE);
  }

  // Закрываем дескриптор разделяемой памяти
  if (close(shm_fd) == -1) {
    perror("close");
    exit(EXIT_FAILURE);
  }

  // Закрываем семафор
  if (sem_close(sem) == -1) {
    perror("sem_close");
    exit(EXIT_FAILURE);
  }

  return 0;
}