#include <fcntl.h>
#include <signal.h>
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
  shm_unlink(SHM_NAME);
  exit(0);
}

void handle_signal(int sig) { cleanup(); }

int main() {
  signal(SIGINT, handle_signal);

  // Создаем сегмент разделяемой памяти
  shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
  if (shm_fd < 0) {
    perror("shm_open");
    exit(1);
  }

  // Устанавливаем размер разделяемой памяти
  if (ftruncate(shm_fd, SHM_SIZE) < 0) {
    perror("ftruncate");
    exit(1);
  }

  // Присоединяем сегмент разделяемой памяти
  shm = (SharedMemory *)mmap(NULL, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED,
                             shm_fd, 0);
  if (shm == MAP_FAILED) {
    perror("mmap");
    exit(1);
  }

  // Инициализируем разделяемую память
  shm->message_count = 0;
  shm->client_count = 0;

  printf("Сервер запущен. Ожидание клиентов...\n");

  while (1) {
    // Сервер просто ждет, пока клиенты будут подключаться и отправлять
    // сообщения
    sleep(1);
  }

  cleanup();
  return 0;
}