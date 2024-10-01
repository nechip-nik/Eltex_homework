#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/local_socket"
#define BUFFER_SIZE 1024

int main() {
  int server_fd, new_socket;
  struct sockaddr_un address;
  int addrlen = sizeof(address);
  char buffer[BUFFER_SIZE] = {0};
  const char *hello = "Привет от сервера";

  if ((server_fd = socket(AF_UNIX, SOCK_STREAM, 0)) == 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  memset(&address, 0, sizeof(address));
  address.sun_family = AF_UNIX;
  strncpy(address.sun_path, SOCKET_PATH, sizeof(address.sun_path) - 1);

  if (bind(server_fd, (struct sockaddr *)&address, sizeof(address)) < 0) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  if (listen(server_fd, 3) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                           (socklen_t *)&addrlen)) < 0) {
    perror("accept");
    exit(EXIT_FAILURE);
  }
  ssize_t bytes_received = recv(new_socket, buffer, BUFFER_SIZE, 0);
  if (bytes_received < 0) {
    perror("recv");
    exit(EXIT_FAILURE);
  }

  printf("%s\n", buffer);

  send(new_socket, hello, strlen(hello), 0);
  close(new_socket);
  close(server_fd);

  unlink(SOCKET_PATH);

  return 0;
}