#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void *handle_client(void *arg) {
  int client_socket = *(int *)arg;
  char buffer[BUFFER_SIZE];
  int bytes_received;

  while ((bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0) {
    buffer[bytes_received] = '\0';
    printf("Полученно сообщение : %s", buffer);
    send(client_socket, buffer, bytes_received, 0);
  }

  if (bytes_received == 0) {
    printf("Клиент отключился\n");
  } else {
    perror("recv");
    exit(EXIT_FAILURE);
  }

  close(client_socket);
  free(arg);
  return NULL;
}

int main() {
  int server_socket, client_socket;
  struct sockaddr_in server_addr, client_addr;
  socklen_t client_addr_len = sizeof(client_addr);
  pthread_t thread_id;

  if ((server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT);

  if (bind(server_socket, (struct sockaddr *)&server_addr,
           sizeof(server_addr)) == -1) {
    perror("bind");
    exit(EXIT_FAILURE);
  }

  if (listen(server_socket, 5) == -1) {
    perror("listen");
    exit(EXIT_FAILURE);
  }

  while (1) {
    if ((client_socket = accept(server_socket, (struct sockaddr *)&client_addr,
                                &client_addr_len)) == -1) {
      perror("accept");
      continue;
    }

    printf("Подлючение к клиенту %s:%d\n", inet_ntoa(client_addr.sin_addr),
           ntohs(client_addr.sin_port));

    int *client_socket_ptr = malloc(sizeof(int));
    *client_socket_ptr = client_socket;

    if (pthread_create(&thread_id, NULL, handle_client, client_socket_ptr) !=
        0) {
      perror("pthread_create");
      free(client_socket_ptr);
      close(client_socket);
    }
  }

  close(server_socket);
  return 0;
}