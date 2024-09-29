#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 8888
#define BUFFER_SIZE 1024

int main() {
  int sockfd;
  struct sockaddr_in server_addr, client_addr;
  char buffer[BUFFER_SIZE];
  socklen_t addr_len = sizeof(client_addr);

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  server_addr.sin_addr.s_addr = INADDR_ANY;

  if (bind(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
    perror("bind");
    close(sockfd);
    exit(EXIT_FAILURE);
  }
  printf("Клиент ждет сообщения\n");

  ssize_t num_bytes = recvfrom(sockfd, buffer, BUFFER_SIZE - 1, 0,
                               (struct sockaddr *)&client_addr, &addr_len);
  if (num_bytes < 0) {
    perror("broadcast");
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  buffer[num_bytes] = '\0';

  printf("Сообщение: %s\n", buffer);
  close(sockfd);

  return 0;
}