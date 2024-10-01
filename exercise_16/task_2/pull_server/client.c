#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("Нужно ввести : %s <listener_ip> <listener_port>\n", argv[0]);
    return 1;
  }

  int client_socket;
  struct sockaddr_in listener_addr;
  char buffer[BUFFER_SIZE];

  client_socket = socket(AF_INET, SOCK_STREAM, 0);
  listener_addr.sin_family = AF_INET;
  listener_addr.sin_port = htons(atoi(argv[2]));
  inet_pton(AF_INET, argv[1], &listener_addr.sin_addr);

  if (connect(client_socket, (struct sockaddr *)&listener_addr,
              sizeof(listener_addr)) < 0) {
    perror("connect");
    return 1;
  }
  int bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
  buffer[bytes_received] = '\0';
  close(client_socket);
  char *message_server_ip = strtok(buffer, ":");
  char *message_server_port = strtok(NULL, ":");
  client_socket = socket(AF_INET, SOCK_STREAM, 0);
  struct sockaddr_in message_server_addr;
  message_server_addr.sin_family = AF_INET;
  message_server_addr.sin_port = htons(atoi(message_server_port));
  inet_pton(AF_INET, message_server_ip, &message_server_addr.sin_addr);

  if (connect(client_socket, (struct sockaddr *)&message_server_addr,
              sizeof(message_server_addr)) < 0) {
    perror("connect");
    return 1;
  }
  while (1) {
    printf("Введите сообщение: ");
    fgets(buffer, BUFFER_SIZE, stdin);
    send(client_socket, buffer, strlen(buffer), 0);

    bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0);
    buffer[bytes_received] = '\0';
    printf("Сообщение полученно: %s\n", buffer);
  }
  close(client_socket);
  return 0;
}