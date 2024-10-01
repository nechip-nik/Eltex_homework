#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/udp_socket"
#define BUFFER_SIZE 1024

int main() {
  int sockfd;
  struct sockaddr_un server_addr, client_addr;
  socklen_t client_addr_len = sizeof(client_addr);
  char buffer[BUFFER_SIZE];
  if ((sockfd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sun_family = AF_UNIX;
  strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

  unlink(SOCKET_PATH);

  if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) ==
      -1) {
    perror("bind");
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  printf("Сервер запущен и ожидает сообщений...\n");

  ssize_t num_bytes =
      recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0,
               (struct sockaddr *)&client_addr, &client_addr_len);
  if (num_bytes == -1) {
    perror("recvfrom");
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  buffer[num_bytes] = '\0';
  printf("%s\n", buffer);

  const char *response = "Привет от сервера";
  if (sendto(sockfd, response, strlen(response), 0,
             (struct sockaddr *)&client_addr, client_addr_len) == -1) {
    perror("sendto");
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  close(sockfd);

  unlink(SOCKET_PATH);

  return 0;
}