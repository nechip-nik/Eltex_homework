#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/udp_socket"
#define CLIENT_SOCKET_PATH "/tmp/udp_client_socket"
#define BUFFER_SIZE 1024

int main() {
  int sockfd;
  struct sockaddr_un server_addr, client_addr;
  char buffer[BUFFER_SIZE];
  if ((sockfd = socket(AF_UNIX, SOCK_DGRAM, 0)) == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  memset(&client_addr, 0, sizeof(client_addr));
  client_addr.sun_family = AF_UNIX;
  strncpy(client_addr.sun_path, CLIENT_SOCKET_PATH,
          sizeof(client_addr.sun_path) - 1);

  unlink(CLIENT_SOCKET_PATH);
  if (bind(sockfd, (struct sockaddr *)&client_addr, sizeof(client_addr)) ==
      -1) {
    perror("bind");
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sun_family = AF_UNIX;
  strncpy(server_addr.sun_path, SOCKET_PATH, sizeof(server_addr.sun_path) - 1);

  const char *message = "Привет от клиента!";
  if (sendto(sockfd, message, strlen(message), 0,
             (struct sockaddr *)&server_addr, sizeof(server_addr)) == -1) {
    perror("sendto");
    close(sockfd);
    exit(EXIT_FAILURE);
  }
  socklen_t server_addr_len = sizeof(server_addr);
  ssize_t num_bytes =
      recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0,
               (struct sockaddr *)&server_addr, &server_addr_len);
  if (num_bytes == -1) {
    perror("recvfrom");
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  buffer[num_bytes] = '\0';
  printf("%s\n", buffer);
  close(sockfd);
  unlink(CLIENT_SOCKET_PATH);
  return 0;
}