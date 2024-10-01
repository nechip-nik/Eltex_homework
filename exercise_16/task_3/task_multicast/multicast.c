#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 8888
#define MULTICAST_IP "224.0.0.1"
#define BUFFER_SIZE 1024

int main() {
  int sockfd;
  struct sockaddr_in multicast_addr;
  char *message = "Hello!!!";
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);
  if (sockfd < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }
  memset(&multicast_addr, 0, sizeof(multicast_addr));
  multicast_addr.sin_family = AF_INET;
  multicast_addr.sin_port = htons(PORT);
  multicast_addr.sin_addr.s_addr = inet_addr(MULTICAST_IP);

  if (sendto(sockfd, message, strlen(message), 0,
             (struct sockaddr *)&multicast_addr, sizeof(multicast_addr)) < 0) {
    perror("multicast");
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  printf("Сообщение отрпавлено на %s:%d\n", MULTICAST_IP, PORT);

  close(sockfd);

  return 0;
}