#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <net/ethernet.h>
#include <net/if.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define BUF_SIZE 1024
#define SERVER_IP "10.42.0.38"
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

uint16_t calculate_checksum(uint16_t *addr, int len) {
  int nleft = len;
  uint32_t sum = 0;
  uint16_t *w = addr;
  uint16_t answer = 0;

  while (nleft > 1) {
    sum += *w++;
    nleft -= 2;
  }

  if (nleft == 1) {
    *(uint8_t *)(&answer) = *(uint8_t *)w;
    sum += answer;
  }

  sum = (sum >> 16) + (sum & 0xffff);
  sum += (sum >> 16);
  answer = ~sum;
  return answer;
}

int main() {
  int fd;
  struct sockaddr_ll serv_addr;
  char buf[BUF_SIZE];
  struct udp_header *udp_hdr;
  ip_header_t *ip_header;
  struct ethhdr *eth_header;
  int index;

  // Создание raw сокета
  fd = socket(AF_PACKET, SOCK_RAW, htons(ETH_P_ALL));
  if (fd < 0) {
    perror("socket");
    exit(EXIT_FAILURE);
  }

  index = if_nametoindex("wlp4s0");
  if (index == 0) {
    perror("if_nametoindex");
    close(fd);
    exit(EXIT_FAILURE);
  }

  // Заполнение Ethernet заголовка
  eth_header = (struct ethhdr *)buf;
  unsigned char src_mac[6] = {0x80, 0x30, 0x49,
                              0x46, 0xea, 0x09}; // MAC-адрес 08:97:98:aa:d6:e5
  unsigned char dst_mac[6] = {0xe4, 0xaa, 0xea, 0xf2, 0xbc, 0x31};
  memcpy(eth_header->h_source, src_mac, 6);
  memcpy(eth_header->h_dest, dst_mac, 6);
  eth_header->h_proto = htons(ETH_P_IP);

  // Заполнение IP заголовка
  ip_header = (ip_header_t *)(buf + sizeof(struct ethhdr));
  ip_header->version_ihl = (4 << 4) | 5;
  ip_header->tos = 0;
  ip_header->total_length = htons(
      sizeof(ip_header_t) + sizeof(struct udp_header) + strlen("hello") + 1);
  ip_header->id = htons(1);
  ip_header->flags_fragment_offset = 0;
  ip_header->ttl = 255;
  ip_header->protocol = IPPROTO_UDP;
  ip_header->checksum = 0;
  ip_header->src_addr = inet_addr("10.42.0.1");
  ip_header->dst_addr = inet_addr("10.42.0.38");
  ip_header->checksum =
      calculate_checksum((uint16_t *)ip_header, sizeof(ip_header_t));

  // Заполнение UDP заголовка
  udp_hdr =
      (struct udp_header *)(buf + sizeof(struct ethhdr) + sizeof(ip_header_t));
  udp_hdr->src_port = htons(8765);
  udp_hdr->dst_port = htons(SERVER_PORT);
  udp_hdr->length = htons(sizeof(struct udp_header) + strlen("hello") + 1);
  udp_hdr->checksum = 0;

  // Заполнение данных
  strcpy(buf + sizeof(struct ethhdr) + sizeof(ip_header_t) +
             sizeof(struct udp_header),
         "hello");

  // Настройка адреса сервера
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sll_family = AF_PACKET;
  serv_addr.sll_ifindex = index;
  serv_addr.sll_halen = 6;
  memcpy(serv_addr.sll_addr, dst_mac, 6);

  // Отправка пакета
  if (sendto(fd, buf,
             sizeof(struct ethhdr) + sizeof(ip_header_t) +
                 sizeof(struct udp_header) + strlen("hello") + 1,
             0, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0) {
    perror("sendto");
    close(fd);
    exit(EXIT_FAILURE);
  }
  char recev[BUF_SIZE];
  printf("Packet sent successfully.\n");

  // Закрытие сокета
  close(fd);
  return 0;
}