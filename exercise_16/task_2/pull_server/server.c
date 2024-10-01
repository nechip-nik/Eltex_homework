#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_MESSAGE_SERVERS 5
#define LISTEN_PORT 8080
#define BUFFER_SIZE 1024

typedef struct {
  int socket;
  int is_free;
  pthread_t thread_id;
} MessageServer;

MessageServer message_servers[MAX_MESSAGE_SERVERS];
pthread_mutex_t lock;

void *handle_client(void *arg) {
  int client_socket = *(int *)arg;
  free(arg);

  char buffer[BUFFER_SIZE];
  int bytes_received;

  while ((bytes_received = recv(client_socket, buffer, BUFFER_SIZE, 0)) > 0) {
    buffer[bytes_received] = '\0';
    printf("Сообщение полученно: %s\n", buffer);
    send(client_socket, buffer, bytes_received, 0);
  }

  close(client_socket);
  return NULL;
}

void *message_server_thread(void *arg) {
  MessageServer *server = (MessageServer *)arg;
  struct sockaddr_in client_addr;
  socklen_t client_addr_len = sizeof(client_addr);
  int client_socket;

  while (1) {
    client_socket = accept(server->socket, (struct sockaddr *)&client_addr,
                           &client_addr_len);
    if (client_socket < 0) {
      perror("accept");
      continue;
    }

    int *client_socket_ptr = malloc(sizeof(int));
    *client_socket_ptr = client_socket;
    pthread_create(&server->thread_id, NULL, handle_client, client_socket_ptr);
    pthread_detach(server->thread_id);
  }

  close(server->socket);
  return NULL;
}

void *listener_thread(void *arg) {
  int listener_socket = *(int *)arg;
  struct sockaddr_in client_addr;
  socklen_t client_addr_len = sizeof(client_addr);
  int client_socket;

  while (1) {
    client_socket = accept(listener_socket, (struct sockaddr *)&client_addr,
                           &client_addr_len);
    if (client_socket < 0) {
      perror("accept");
      continue;
    }
    int message_server_socket = -1;
    pthread_mutex_lock(&lock);
    for (int i = 0; i < MAX_MESSAGE_SERVERS; i++) {
      if (message_servers[i].is_free) {
        message_servers[i].is_free = 0;
        message_server_socket = message_servers[i].socket;
        break;
      }
    }
    pthread_mutex_unlock(&lock);

    if (message_server_socket == -1) {
      close(client_socket);
      continue;
    }
    struct sockaddr_in message_server_addr;
    socklen_t len = sizeof(message_server_addr);
    getsockname(message_server_socket, (struct sockaddr *)&message_server_addr,
                &len);
    char message_server_ip[INET_ADDRSTRLEN];
    inet_ntop(AF_INET, &message_server_addr.sin_addr, message_server_ip,
              INET_ADDRSTRLEN);
    int message_server_port = ntohs(message_server_addr.sin_port);

    char response[50];
    snprintf(response, sizeof(response), "%s:%d", message_server_ip,
             message_server_port);
    send(client_socket, response, strlen(response), 0);

    close(client_socket);
  }

  close(listener_socket);
  return NULL;
}

int main() {
  int listener_socket;
  struct sockaddr_in listener_addr;
  for (int i = 0; i < MAX_MESSAGE_SERVERS; i++) {
    message_servers[i].socket = socket(AF_INET, SOCK_STREAM, 0);
    message_servers[i].is_free = 1;

    struct sockaddr_in message_server_addr;
    message_server_addr.sin_family = AF_INET;
    message_server_addr.sin_addr.s_addr = INADDR_ANY;
    message_server_addr.sin_port = htons(LISTEN_PORT + i + 1);

    if (bind(message_servers[i].socket, (struct sockaddr *)&message_server_addr,
             sizeof(message_server_addr)) < 0) {
      perror("bind");
      exit(EXIT_FAILURE);
    }

    if (listen(message_servers[i].socket, 1) < 0) {
      perror("listen");
      exit(EXIT_FAILURE);
    }

    pthread_create(&message_servers[i].thread_id, NULL, message_server_thread,
                   &message_servers[i]);
  }

  // Create listener socket
  listener_socket = socket(AF_INET, SOCK_STREAM, 0);
  listener_addr.sin_family = AF_INET;
  listener_addr.sin_addr.s_addr = INADDR_ANY;
  listener_addr.sin_port = htons(LISTEN_PORT);

  if (bind(listener_socket, (struct sockaddr *)&listener_addr,
           sizeof(listener_addr)) < 0) {
    perror("bind");
    exit(EXIT_FAILURE);
  }
  printf("Сервер ждет сообщение по 127.0.0.1 8080\n");
  if (listen(listener_socket, 10) < 0) {
    perror("listen");
    exit(EXIT_FAILURE);
  }
  pthread_mutex_init(&lock, NULL);
  pthread_t listener_thread_id;
  int *listener_socket_ptr = malloc(sizeof(int));
  *listener_socket_ptr = listener_socket;
  pthread_create(&listener_thread_id, NULL, listener_thread,
                 listener_socket_ptr);
  pthread_detach(listener_thread_id);
  for (int i = 0; i < MAX_MESSAGE_SERVERS; i++) {
    pthread_join(message_servers[i].thread_id, NULL);
  }

  pthread_mutex_destroy(&lock);
  return 0;
}