#include <arpa/inet.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define MAX_CLIENTS 10
#define BUFFER_SIZE 1024
typedef struct {
  int socket;
  int is_free;
} Worker;
typedef struct {
  int socket;
  char buffer[BUFFER_SIZE];
  int bytes_received;
} QueueItem;
typedef struct {
  QueueItem items[MAX_CLIENTS];
  int front;
  int rear;
  int count;
  pthread_mutex_t mutex;
  pthread_cond_t cond;
} Queue;
Worker workers[MAX_CLIENTS];
Queue queue;
void queue_init(Queue *q) {
  q->front = 0;
  q->rear = 0;
  q->count = 0;
  pthread_mutex_init(&q->mutex, NULL);
  pthread_cond_init(&q->cond, NULL);
}
void queue_push(Queue *q, QueueItem item) {
  pthread_mutex_lock(&q->mutex);
  q->items[q->rear] = item;
  q->rear = (q->rear + 1) % MAX_CLIENTS;
  q->count++;
  pthread_cond_signal(&q->cond);
  pthread_mutex_unlock(&q->mutex);
}
QueueItem queue_pop(Queue *q) {
  pthread_mutex_lock(&q->mutex);
  while (q->count == 0) {
    pthread_cond_wait(&q->cond, &q->mutex);
  }
  QueueItem item = q->items[q->front];
  q->front = (q->front + 1) % MAX_CLIENTS;
  q->count--;
  pthread_mutex_unlock(&q->mutex);
  return item;
}
void *worker_thread(void *arg) {
  Worker *worker = (Worker *)arg;
  char buffer[BUFFER_SIZE];
  int bytes_received;

  while (1) {
    pthread_mutex_lock(&queue.mutex);
    while (worker->is_free) {
      pthread_mutex_unlock(&queue.mutex);
      usleep(10);
      pthread_mutex_lock(&queue.mutex);
    }
    pthread_mutex_unlock(&queue.mutex);

    memset(buffer, 0, BUFFER_SIZE);

    while ((bytes_received = recv(worker->socket, buffer, BUFFER_SIZE, 0)) >
           0) {
      buffer[bytes_received] = '\0';
      printf("Сервер %d получил сообщение: %s\n", worker->socket, buffer);

      QueueItem item;
      item.socket = worker->socket;
      strncpy(item.buffer, buffer, BUFFER_SIZE);
      item.bytes_received = bytes_received;
      queue_push(&queue, item);
      memset(buffer, 0, BUFFER_SIZE);
    }
    if (bytes_received <= 0) {
      close(worker->socket);
      pthread_mutex_lock(&queue.mutex);
      worker->is_free = 1;
      pthread_mutex_unlock(&queue.mutex);
      continue;
    }
    pthread_mutex_lock(&queue.mutex);
    worker->is_free = 1;
    pthread_mutex_unlock(&queue.mutex);
  }

  return NULL;
}

void *processor_thread(void *arg) {
  while (1) {
    QueueItem item = queue_pop(&queue);
    send(item.socket, item.buffer, item.bytes_received, 0);
  }

  return NULL;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "задайте колличество серверов: %s <number_of_workers>\n",
            argv[0]);
    exit(EXIT_FAILURE);
  }

  int num_workers = atoi(argv[1]);
  if (num_workers > MAX_CLIENTS) {
    fprintf(stderr, "Неправильная запись параметра %d\n", MAX_CLIENTS);
    exit(EXIT_FAILURE);
  }

  int server_socket, client_socket;
  struct sockaddr_in server_addr, client_addr;
  socklen_t client_addr_len = sizeof(client_addr);
  server_socket = socket(AF_INET, SOCK_STREAM, 0);
  if (server_socket == -1) {
    perror("socket");
    exit(EXIT_FAILURE);
  }
  server_addr.sin_family = AF_INET;
  server_addr.sin_addr.s_addr = inet_addr("127.0.0.1");
  server_addr.sin_port = htons(8080);

  if (bind(server_socket, (struct sockaddr *)&server_addr,
           sizeof(server_addr)) == -1) {
    perror("bind");
    close(server_socket);
    exit(EXIT_FAILURE);
  }
  if (listen(server_socket, MAX_CLIENTS) == -1) {
    perror("listen");
    close(server_socket);
    exit(EXIT_FAILURE);
  }

  printf("Сервер ждет сообщение 127.0.0.1 8080\n");
  queue_init(&queue);
  pthread_t threads[num_workers];
  for (int i = 0; i < num_workers; i++) {
    workers[i].socket = -1;
    workers[i].is_free = 1;
    if (pthread_create(&threads[i], NULL, worker_thread, &workers[i]) != 0) {
      perror("pthread");
      exit(EXIT_FAILURE);
    }
  }
  pthread_t processor_threads[num_workers];
  for (int i = 0; i < num_workers; i++) {
    if (pthread_create(&processor_threads[i], NULL, processor_thread, NULL) !=
        0) {
      perror("pthread");
      exit(EXIT_FAILURE);
    }
  }
  while (1) {
    client_socket = accept(server_socket, (struct sockaddr *)&client_addr,
                           &client_addr_len);
    if (client_socket == -1) {
      perror("accept");
      continue;
    }
    pthread_mutex_lock(&queue.mutex);
    for (int i = 0; i < num_workers; i++) {
      if (workers[i].is_free) {
        workers[i].socket = client_socket;
        workers[i].is_free = 0;
        break;
      }
    }
    pthread_mutex_unlock(&queue.mutex);
  }
  close(server_socket);
  return 0;
}