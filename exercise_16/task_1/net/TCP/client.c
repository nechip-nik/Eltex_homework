#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
  int sock = 0;
  struct sockaddr_in serv_addr;
  char buffer[BUFFER_SIZE] = {0};
  const char *hello = "Привет от клиента";
  if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(PORT);
  if (inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr) <= 0) {
    perror("address");
    close(sock);
    exit(EXIT_FAILURE);
  }
  if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("connect");
    close(sock);
    exit(EXIT_FAILURE);
  }

  send(sock, hello, strlen(hello), 0);

  int valread = recv(sock, buffer, BUFFER_SIZE, 0);
  if (valread < 0) {
    perror("recv");
    close(sock);
    exit(EXIT_FAILURE);
  }
  printf("%s\n", buffer);

  close(sock);

  return 0;
}