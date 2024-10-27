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
  const char *response = "Hello!";
  shm_fd = shm_open(SHM_NAME, O_RDWR, 0666);
  if (shm_fd == -1) {
    perror("shm_open");
    exit(EXIT_FAILURE);
  }
  ptr = mmap(0, SHM_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, shm_fd, 0);
  if (ptr == MAP_FAILED) {
    perror("mmap");
    exit(EXIT_FAILURE);
  }
  printf("Сообщение от сервера:  %s\n", (char *)ptr);
  memcpy(ptr, response, strlen(response) + 1);

  printf("Полученное сообщение от сервера: %s\n", response);

  if (munmap(ptr, SHM_SIZE) == -1) {
    perror("munmap");
    exit(EXIT_FAILURE);
  }
  if (close(shm_fd) == -1) {
    perror("close");
    exit(EXIT_FAILURE);
  }

  return 0;
}