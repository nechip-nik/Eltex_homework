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

  // Создаем сегмент разделяемой памяти
  shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
  if (shm_fd == -1) {
    perror("shm_open");
    exit(EXIT_FAILURE);
  }

  // Устанавливаем размер сегмента
  if (ftruncate(shm_fd, SHM_SIZE) == -1) {
    perror("ftruncate");
    exit(EXIT_FAILURE);
  }

  // Отображаем сегмент в память
  chat_room = (ChatRoom *)mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
                               shm_fd, 0);
  if (chat_room == MAP_FAILED) {
    perror("mmap");
    exit(EXIT_FAILURE);
  }

  // Инициализируем семафор
  sem = sem_open(SEM_NAME, O_CREAT, 0666, 1);
  if (sem == SEM_FAILED) {
    perror("sem_open");
    exit(EXIT_FAILURE);
  }

  // Инициализируем чат-комнату
  chat_room->client_count = 0;

  printf("Server: Chat room initialized.\n");

  while (1) {
    sem_wait(sem);

    // Проверяем новые сообщения
    for (int i = 0; i < chat_room->client_count; i++) {
      if (chat_room->messages[i].message[0] != '\0') {
        // Рассылаем сообщение всем клиентам
        for (int j = 0; j < chat_room->client_count; j++) {
          if (i != j) {
            strcpy(chat_room->messages[j].message,
                   chat_room->messages[i].message);
          }
        }
        // Очищаем сообщение отправителя
        chat_room->messages[i].message[0] = '\0';
      }
    }

    sem_post(sem);
    sleep(1);
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

  // Удаляем сегмент разделяемой памяти
  if (shm_unlink(SHM_NAME) == -1) {
    perror("shm_unlink");
    exit(EXIT_FAILURE);
  }

  // Закрываем семафор
  if (sem_close(sem) == -1) {
    perror("sem_close");
    exit(EXIT_FAILURE);
  }

  // Удаляем семафор
  if (sem_unlink(SEM_NAME) == -1) {
    perror("sem_unlink");
    exit(EXIT_FAILURE);
  }

  printf("Server: Shared memory segment and semaphore removed.\n");

  return 0;
}