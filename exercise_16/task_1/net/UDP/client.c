#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 8000
#define BUFFER_SIZE 1024

int main() {
  int sockfd;
  struct sockaddr_in servaddr;
  char buffer[BUFFER_SIZE];
  const char *hello = "Привет от клиента";
  ssize_t valread;
  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }
  memset(&servaddr, 0, sizeof(servaddr));
  servaddr.sin_family = AF_INET;
  servaddr.sin_port = htons(PORT);
  if (inet_pton(AF_INET, "127.0.0.1", &servaddr.sin_addr) <= 0) {
    perror("address");
    exit(EXIT_FAILURE);
  }
  sendto(sockfd, (const char *)hello, strlen(hello), MSG_CONFIRM,
         (const struct sockaddr *)&servaddr, sizeof(servaddr));
  socklen_t len = sizeof(servaddr);
  valread = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, MSG_WAITALL,
                     (struct sockaddr *)&servaddr, &len);
  buffer[valread] = '\0';
  printf("%s\n", buffer);
  close(sockfd);
  return 0;
}