#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/local_socket"
#define BUFFER_SIZE 1024

int main() {
  int sock = 0;
  struct sockaddr_un serv_addr;
  char buffer[BUFFER_SIZE] = {0};
  const char *hello = "Привет от клиента";

  if ((sock = socket(AF_UNIX, SOCK_STREAM, 0)) < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sun_family = AF_UNIX;
  strncpy(serv_addr.sun_path, SOCKET_PATH, sizeof(serv_addr.sun_path) - 1);

  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("connect");
    exit(EXIT_FAILURE);
  }

  send(sock, hello, strlen(hello), 0);

  ssize_t bytes_received = recv(sock, buffer, BUFFER_SIZE, 0);
  if (bytes_received < 0) {
    perror("recv");
    exit(EXIT_FAILURE);
  }

  printf("%s\n", buffer);
  close(sock);
  return 0;
}