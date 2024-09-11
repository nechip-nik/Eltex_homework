#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define QUEUE_NAME "/queue"
#define MAX_SIZE 1024

int main() {
  mqd_t mq;
  mq = mq_open(QUEUE_NAME, O_RDWR);
  if (mq == (mqd_t)-1) {
    perror("mq_open");
    exit(EXIT_FAILURE);
  }

  char buffer[MAX_SIZE];
  ssize_t bytes_read;
  if ((bytes_read = mq_receive(mq, buffer, MAX_SIZE, NULL)) == -1) {
    perror("mq_receive");
    exit(EXIT_FAILURE);
  }

  buffer[bytes_read] = '\0';
  printf("Клиент получил сообщение: %s\n", buffer);

  char response[8] = "Hello!";
  if (mq_send(mq, response, strlen(response), 0) == -1) {
    perror("mq_send");
    exit(EXIT_FAILURE);
  }

  printf("Клиент отправил ответ: %s\n", response);

  mq_close(mq);

  return 0;
}