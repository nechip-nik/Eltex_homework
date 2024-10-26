#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
  int sockfd;
  struct sockaddr_in servaddr, cliaddr;
  char buffer[BUFFER_SIZE];
  socklen_t len;
  ssize_t valread;

  if ((sockfd = socket(AF_INET, SOCK_DGRAM, 0)) < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  memset(&servaddr, 0, sizeof(servaddr));
  memset(&cliaddr, 0, sizeof(cliaddr));

  servaddr.sin_family = AF_INET;
  servaddr.sin_addr.s_addr = inet_addr("127.1.0.1");
  servaddr.sin_port = htons(PORT);

  if (bind(sockfd, (const struct sockaddr *)&servaddr, sizeof(servaddr)) < 0) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  len = sizeof(cliaddr);
  printf("Сервер ждет сообщение\n");

  valread = recvfrom(sockfd, (char *)buffer, BUFFER_SIZE, MSG_WAITALL,
                     (struct sockaddr *)&cliaddr, &len);
  buffer[valread] = '\0';
  printf("Получено от клиента: %s\n", buffer);
  strcat(buffer, " from server");
  sendto(sockfd, buffer, strlen(buffer), MSG_CONFIRM,
         (const struct sockaddr *)&cliaddr, len);
  printf("Ответ отправлен клиенту\n");

  close(sockfd);
  return 0;
}