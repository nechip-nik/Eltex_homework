#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>

#define SHM_NAME "/posix_shm"
#define SHM_SIZE 1024

int main() {
  int shm_fd;
  void *ptr;

  shm_fd = shm_open(SHM_NAME, O_CREAT | O_RDWR, 0666);
  if (shm_fd == -1) {
    perror("shm_open");
    exit(EXIT_FAILURE);
  }

  if (ftruncate(shm_fd, SHM_SIZE) == -1) {
    perror("ftruncate");
    exit(EXIT_FAILURE);
  }

  ptr = mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  if (ptr == MAP_FAILED) {
    perror("mmap");
    exit(EXIT_FAILURE);
  }

  const char *message = "Hi!";
  memcpy(ptr, message, strlen(message) + 1);

  printf("Отправил сообщение клиенту: %s\n", message);
  printf("Жду сообщения 10 секунд\n");
  sleep(10); // Ожидание 10 секунд до удаления отображения и сегмента памяти

  printf("Полученно сообщение: %s\n", (char *)ptr);
  if (munmap(ptr, SHM_SIZE) == -1) {
    perror("munmap");
    exit(EXIT_FAILURE);
  }
  if (close(shm_fd) == -1) {
    perror("close");
    exit(EXIT_FAILURE);
  }
  if (shm_unlink(SHM_NAME) == -1) {
    perror("shm_unlink");
    exit(EXIT_FAILURE);
  }

  return 0;
}