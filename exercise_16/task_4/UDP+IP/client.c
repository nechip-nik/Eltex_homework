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

typedef struct {
  uint8_t version_ihl;
  uint8_t tos;
  uint16_t total_length;
  uint16_t id;
  uint16_t flags_fragment_offset;
  uint8_t ttl;
  uint8_t protocol;
  uint16_t checksum;
  uint32_t src_addr;
  uint32_t dst_addr;
} ip_header_t;

int main() {
  int fd;
  struct sockaddr_in serv_addr;
  char buf[BUF_SIZE];
  struct udp_header *udp_hdr;
  ip_header_t *ip_header;

  ip_header = (ip_header_t *)buf;
  ip_header->version_ihl = (4 << 4) | 5;
  ip_header->tos = 0;
  ip_header->total_length = 0;
  ip_header->flags_fragment_offset = 0;
  ip_header->ttl = 255;
  ip_header->protocol = IPPROTO_UDP;
  ip_header->checksum = 0;
  ip_header->src_addr = inet_addr("127.0.0.1");
  ip_header->dst_addr = inet_addr("127.1.0.1");
  ip_header->checksum = 0;

  udp_hdr = (struct udp_header *)(buf + sizeof(ip_header_t));
  udp_hdr->src_port = htons(8765);
  udp_hdr->dst_port = htons(SERVER_PORT);
  udp_hdr->length = htons(sizeof(struct udp_header) + strlen("hello") + 1);
  udp_hdr->checksum = 0;

  strcpy(buf + sizeof(ip_header_t) + sizeof(struct udp_header), "hello");

  fd = socket(AF_INET, SOCK_RAW, IPPROTO_UDP);
  if (fd < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  int optval = 1;
  if (setsockopt(fd, IPPROTO_IP, IP_HDRINCL, &optval, sizeof(optval)) < 0) {
    perror("setsockopt");
    close(fd);
    exit(EXIT_FAILURE);
  }

  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port = htons(SERVER_PORT);
  serv_addr.sin_addr.s_addr = inet_addr(SERVER_IP);

  if (sendto(fd, buf,
             sizeof(ip_header_t) + sizeof(struct udp_header) + strlen("hello") +
                 1,
             0, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("sendto");
    close(fd);
    exit(EXIT_FAILURE);
  }

  char recev[BUF_SIZE];

  while (1) {
    int recv_len;
    struct sockaddr_in from_addr;
    socklen_t addr_len = sizeof(from_addr);
    recv_len = recvfrom(fd, recev, BUF_SIZE, 0, (struct sockaddr *)&from_addr,
                        &addr_len);
    if (recv_len < 0) {
      perror("recvfrom");
      close(fd);
      exit(EXIT_FAILURE);
    }
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