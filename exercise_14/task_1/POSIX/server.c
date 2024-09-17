#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int main() {
  int shm_fd;
  char *shm_ptr;

  shm_fd = shm_open("/shm", O_CREAT | O_RDWR, 0666);
}