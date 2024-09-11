#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define QUEUE_NAME "/queue"
#define MAX_SIZE 1024

int main() {
  mqd_t mq;
  struct mq_attr attr;
  attr.mq_flags = 0;
  attr.mq_maxmsg = 10;
  attr.mq_msgsize = MAX_SIZE;
  attr.mq_curmsgs = 0;

  mq = mq_open(QUEUE_NAME, O_CREAT | O_RDWR, 0644, &attr);
  if (mq == (mqd_t)-1) {
    perror("mq_open");
    exit(EXIT_FAILURE);
  }

  char message[4] = "Hi!";
  if (mq_send(mq, message, strlen(message), 0) == -1) {
    perror("mq_send");
    exit(EXIT_FAILURE);
  }

  printf("Сервер отправил сообщение: %s\n", message);
  sleep(10);

  char buffer[MAX_SIZE];
  ssize_t bytes_read;
  if ((bytes_read = mq_receive(mq, buffer, MAX_SIZE, NULL)) == -1) {
    perror("mq_receive");
    exit(EXIT_FAILURE);
  }

  buffer[bytes_read] = '\0';
  printf("Сервер получил ответ: %s\n", buffer);

  printf("Сервер завершил работу\n");
  mq_close(mq);
  mq_unlink(QUEUE_NAME);

  return 0;
}