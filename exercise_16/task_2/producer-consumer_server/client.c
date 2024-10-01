#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
  if (argc != 3) {
    fprintf(stderr, "Используйте: %s <server_ip> <server_port>\n", argv[0]);
    exit(EXIT_FAILURE);
  }

  const char *server_ip = argv[1];
  int server_port = atoi(argv[2]);

  int client_socket;
  struct sockaddr_in server_addr;
  char buffer[BUFFER_SIZE];
  client_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (client_socket == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(server_port);
  if (inet_pton(AF_INET, server_ip, &server_addr.sin_addr) <= 0) {
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

  printf("Подлючение к серверу %s:%d\n", server_ip, server_port);

  while (1) {
    printf("Введите сообщение: ");
    fgets(buffer, BUFFER_SIZE, stdin);
    buffer[strcspn(buffer, "\n")] = 0;

    if (send(client_socket, buffer, strlen(buffer), 0) == -1) {
      perror("Send");
      break;
    }
    int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
    if (bytes_received <= 0) {
      if (bytes_received == 0) {
        printf("Сервер отключился\n");
      } else {
        perror("recv");
      }
      break;
    }
    buffer[bytes_received] = '\0';
    printf("Сообщение полученно: %s\n", buffer);
  }
  close(client_socket);
  return 0;
}