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
  client_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (client_socket < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }
  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_port = htons(PORT);
  if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0) {
    perror("address");
    exit(EXIT_FAILURE);
  }

  if (connect(client_socket, (struct sockaddr *)&server_addr,
              sizeof(server_addr)) < 0) {
    perror("connect");
    exit(EXIT_FAILURE);
  }
  const char *request = "Запрос от TCP клиента";
  send(client_socket, request, strlen(request), 0);

  memset(buffer, 0, BUFFER_SIZE);
  int bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
  if (bytes_received < 0) {
    perror("recv");
  } else {
    printf("Ответ от сервера: %s\n", buffer);
  }

  close(client_socket);

  return 0;
}