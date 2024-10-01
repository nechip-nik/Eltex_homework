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

  client_socket = socket(AF_INET, SOCK_DGRAM, 0);
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
  const char *request = "Запрос от UDP клиента";
  sendto(client_socket, request, strlen(request), 0,
         (struct sockaddr *)&server_addr, sizeof(server_addr));
  memset(buffer, 0, BUFFER_SIZE);
  socklen_t addr_len = sizeof(server_addr);
  int bytes_received = recvfrom(client_socket, buffer, BUFFER_SIZE - 1, 0,
                                (struct sockaddr *)&server_addr, &addr_len);
  if (bytes_received < 0) {
    perror("recv");
  } else {
    printf("Ответ от сервера: %s\n", buffer);
  }

  close(client_socket);

  return 0;
}