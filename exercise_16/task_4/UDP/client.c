#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 1024
#define SERVER_IP "127.1.0.1"
#define SERVER_PORT 8080

struct udp_header {
  uint16_t src_port;
  uint16_t dst_port;
  uint16_t length;
  uint16_t checksum;
};

int main() {
  int fd;
  struct sockaddr_in serv_addr;
  char buf[BUF_SIZE];
  struct udp_header *udp_hdr;

  fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
  if (fd < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(SERVER_PORT);
  serv_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

  udp_hdr = (struct udp_header *)buf;
  udp_hdr->src_port = htons(8765);
  udp_hdr->dst_port = htons(SERVER_PORT);
  udp_hdr->length = htons(sizeof(struct udp_header) + strlen("hello") + 1);
  udp_hdr->checksum = 0;

  strcpy(buf + sizeof(struct udp_header), "hello");

  // Отправка пакета

  if (sendto(fd, buf, sizeof(struct udp_header) + strlen("hello") + 1, 0,
             (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("sendto");
    close(fd);
    exit(EXIT_FAILURE);
  }

  char recev[BUF_SIZE];

  // Бесконечный цикл для получения ответов
  while (1) {
    int recv_len;
    struct sockaddr_in from_addr;
    socklen_t addr_len = sizeof(from_addr);
    printf("recv\n");
    recv_len = recvfrom(fd, recev, BUF_SIZE, 0, (struct sockaddr *)&from_addr,
                        &addr_len);
    if (recv_len < 0) {
      perror("recvfrom");
      close(fd);
      exit(EXIT_FAILURE);
    }
    printf("end_recv\n");
    printf("Received %d bytes from %s:%d\n", recv_len,
           inet_ntoa(from_addr.sin_addr), ntohs(from_addr.sin_port));
    printf("Data: %s\n", recev + 28);

    if ((from_addr.sin_addr.s_addr == inet_addr(SERVER_IP)) &&
        (from_addr.sin_port == htons(0))) {
      printf("Received %d bytes from %s:%d\n", recv_len,
             inet_ntoa(from_addr.sin_addr), ntohs(from_addr.sin_port));
      printf("Data: %s\n", recev + 28);
      close(fd);
      return 0;
    }
  }

  close(fd);
  return 0;
}