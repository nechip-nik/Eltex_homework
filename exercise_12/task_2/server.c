#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

int main() {
  if (mkfifo("fifo", 0666) == -1) {
    perror("mkfifo");
    exit(EXIT_FAILURE);
  }
  int fd = open("fifo", O_WRONLY);
  if (fd == -1) {
    perror("open");
    exit(EXIT_FAILURE);
  }
  const char message[4] = "Hi!";
  write(fd, message, strlen(message));
  close(fd);
  printf("Сервер завершил работу\n");
}