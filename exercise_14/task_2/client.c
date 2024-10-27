#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define SHM_NAME "/chat_shm"
#define SHM_SIZE 1024 // Размер разделяемой памяти
#define MAX_MESSAGES 100 // Максимальное количество сообщений
#define MAX_CLIENTS 10 // Максимальное количество клиентов
#define MSG_SIZE 100 // Максимальный размер сообщения

typedef struct {
  char messages[MAX_MESSAGES][MSG_SIZE];
  int message_count;
  char clients[MAX_CLIENTS][MSG_SIZE];
  int client_count;
} SharedMemory;

SharedMemory *shm;
int shm_fd;

void cleanup() {
  munmap(shm, SHM_SIZE);
  exit(0);
}

int main(int argc, char *argv[]) {
  if (argc < 2) {
    printf("Использование: %s <имя_клиента>\n", argv[0]);
    exit(1);
  }

  char *client_name = argv[1];

  // Получаем идентификатор сегмента разделяемой памяти
  shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
  if (shm_fd < 0) {
    perror("shm_open");
    exit(1);
  }

  // Присоединяем сегмент разделяемой памяти
  shm = (SharedMemory *)mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
                             shm_fd, 0);
  if (shm == MAP_FAILED) {
    perror("mmap");
    exit(1);
  }

  // Добавляем клиента в список
  if (shm->client_count < MAX_CLIENTS) {
    strncpy(shm->clients[shm->client_count], client_name, MSG_SIZE);
    shm->client_count++;
  } else {
    printf("Слишком много клиентов. Отключение...\n");
    cleanup();
  }

  // Выводим все сообщения в комнате
  for (int i = 0; i < shm->message_count; i++) {
    printf("%s\n", shm->messages[i]);
  }

  // Основной цикл клиента
  while (1) {
    char message[MSG_SIZE];
    printf("Введите сообщение: ");
    fgets(message, MSG_SIZE, stdin);

    // Удаляем символ новой строки
    message[strcspn(message, "\n")] = 0;

    // Добавляем сообщение в разделяемую память
    if (shm->message_count < MAX_MESSAGES) {
      snprintf(shm->messages[shm->message_count], MSG_SIZE, "%s: %s",
               client_name, message);
      shm->message_count++;
    } else {
      printf("Слишком много сообщений. Отключение...\n");
      cleanup();
    }
  }

  cleanup();
  return 0;
}