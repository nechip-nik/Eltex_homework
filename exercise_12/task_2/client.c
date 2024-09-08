#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int main() {
  int fd = open("fifo", O_RDONLY);
  if (fd == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }
  char buffer[10];
  int message = read(fd, buffer, sizeof(buffer));
  if (message > 0) {
    printf("%s\n", buffer);
  } else {
    perror("read");
  }
  close(fd);
  if (unlink("fifo") == -1) {
    perror("unlink");
    exit(EXIT_FAILURE);
  }
  printf("Клиент завершил работу\n");
}