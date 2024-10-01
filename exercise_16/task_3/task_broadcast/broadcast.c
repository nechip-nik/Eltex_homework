#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 8888
#define BROADCAST_IP "255.255.255.255"
#define BUFFER_SIZE 1024

int main() {
  int sockfd;
  struct sockaddr_in broadcast_addr;
  char *message = "Hello!!!!";

  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    perror("Socket");
    exit(EXIT_FAILURE);
  }

  memset(&broadcast_addr, 0, sizeof(broadcast_addr));
  broadcast_addr.sin_family = AF_INET;
  broadcast_addr.sin_port = htons(PORT);
  broadcast_addr.sin_addr.s_addr = inet_addr(BROADCAST_IP);

  int broadcastEnable = 1;
  if (setsockopt(sockfd, SOL_SOCKET, SO_BROADCAST, &broadcastEnable,
                 sizeof(broadcastEnable)) < 0) {
    perror("option");
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  if (sendto(sockfd, message, strlen(message), 0,
             (struct sockaddr *)&broadcast_addr, sizeof(broadcast_addr)) < 0) {
    perror("broadcast");
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  printf("Сообщение отправлено на порт %d\n", PORT);

  close(sockfd);

  return 0;
}