#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1"
#define PORT 8080
#define BUFFER_SIZE 1024

int main() {
  int client_socket;
  struct sockaddr_in server_addr;
  char buffer[BUFFER_SIZE];
  int bytes_received;

  if ((client_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);

  if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
    perror("address");
    close(client_socket);
    exit(EXIT_FAILURE);
  }

  if (connect(client_socket, (struct sockaddr *)&server_addr,
              sizeof(server_addr)) == -1) {
    perror("connect");
    close(client_socket);
    exit(EXIT_FAILURE);
  }

  printf("Подключение в серверу %s:%d\n", SERVER_IP, PORT);

  while (1) {
    printf("Введите сообщение: ");
    fgets(buffer, BUFFER_SIZE, stdin);

    if (send(client_socket, buffer, strlen(buffer), 0) == -1) {
      perror("send");
      exit(EXIT_FAILURE);
    }

    bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
    if (bytes_received <= 0) {
      if (bytes_received == 0) {
        printf("Отключение от сервера\n");
      } else {
        perror("recv");
        exit(EXIT_FAILURE);
      }
      break;
    }

    buffer[bytes_received] = '\0';
    printf("Сообщение полученно : %s", buffer);
  }

  close(client_socket);
  return 0;
}