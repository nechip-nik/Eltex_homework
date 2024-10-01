#include <arpa/inet.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define PORT 8080
#define BUFFER_SIZE 1024

void handle_tcp_client(int client_socket) {
  char buffer[BUFFER_SIZE];
  memset(buffer, 0, BUFFER_SIZE);

  int bytes_received = recv(client_socket, buffer, BUFFER_SIZE - 1, 0);
  if (bytes_received < 0) {
    perror("recv");
    exit(EXIT_FAILURE);
  }

  printf("Получен TCP запрос: %s\n", buffer);

  const char *response = "Ответ от TCP сервера";
  send(client_socket, response, strlen(response), 0);

  close(client_socket);
}

void handle_udp_client(int udp_socket, struct sockaddr_in client_addr,
                       socklen_t addr_len) {
  char buffer[BUFFER_SIZE];
  memset(buffer, 0, BUFFER_SIZE);

  int bytes_received = recvfrom(udp_socket, buffer, BUFFER_SIZE - 1, 0,
                                (struct sockaddr *)&client_addr, &addr_len);
  if (bytes_received < 0) {
    perror("recv");
    exit(EXIT_FAILURE);
  }

  printf("Получен UDP запрос: %s\n", buffer);
  const char *response = "Ответ от UDP сервера";
  sendto(udp_socket, response, strlen(response), 0,
         (struct sockaddr *)&client_addr, addr_len);
}

int main() {
  int tcp_socket, udp_socket, client_socket;
  struct sockaddr_in server_addr, client_addr;
  socklen_t addr_len = sizeof(client_addr);

  tcp_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (tcp_socket < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  udp_socket = socket(AF_INET, SOCK_DGRAM, 0);
  if (udp_socket < 0) {
    perror("socket");
    close(tcp_socket);
    exit(EXIT_FAILURE);
  }

  memset(&server_addr, 0, sizeof(server_addr));
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = INADDR_ANY;
  server_addr.sin_port = htons(PORT);

  if (bind(tcp_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) {
    perror("bind_tcp");
    close(tcp_socket);
    close(udp_socket);
    exit(EXIT_FAILURE);
  }

  if (bind(udp_socket, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
      0) {
    perror("bind_udp");
    close(tcp_socket);
    close(udp_socket);
    exit(EXIT_FAILURE);
  }

  if (listen(tcp_socket, 5) < 0) {
    perror("listen");
    close(tcp_socket);
    close(udp_socket);
    exit(EXIT_FAILURE);
  }

  printf("Сервер запущен на порту %d\n", PORT);

  while (1) {
    fd_set read_fds;
    int max_fd;

    FD_ZERO(&read_fds);
    FD_SET(tcp_socket, &read_fds);
    FD_SET(udp_socket, &read_fds);

    max_fd = (tcp_socket > udp_socket) ? tcp_socket : udp_socket;

    if (select(max_fd + 1, &read_fds, NULL, NULL, NULL) < 0) {
      perror("select");
      continue;
    }

    if (FD_ISSET(tcp_socket, &read_fds)) {
      client_socket =
          accept(tcp_socket, (struct sockaddr *)&client_addr, &addr_len);
      if (client_socket < 0) {
        perror("accept");
        continue;
      }
      handle_tcp_client(client_socket);
    }

    if (FD_ISSET(udp_socket, &read_fds)) {
      handle_udp_client(udp_socket, client_addr, addr_len);
    }
  }

  close(tcp_socket);
  close(udp_socket);

  return 0;
}