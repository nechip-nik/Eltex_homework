#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>

#define QUEUE_NAME_WRITE "/queue_write"
#define QUEUE_NAME_READ "/queue_read"
#define MAX_CLIENTS 10
#define MAX_SIZE 1024

typedef struct {
    mqd_t mq;
    char name[256];
} Client;

Client clients[MAX_CLIENTS];
int client_count = 0;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
void* receive_message(void* arg) {
    mqd_t mq_read = *(mqd_t*)arg;
    char buffer[MAX_SIZE];
    ssize_t bytes_read;

    while ((bytes_read = mq_receive(mq_read, buffer, MAX_SIZE, NULL)) != -1) {
        buffer[bytes_read] = '\0';
        printf("Сервер получил сообщение: %s\n", buffer);
        pthread_mutex_lock(&clients_mutex);
        if (client_count < MAX_CLIENTS) {
            clients[client_count].mq = mq_open(buffer, O_WRONLY);
            if (clients[client_count].mq != (mqd_t)-1) {
                strncpy(clients[client_count].name, buffer, sizeof(clients[client_count].name));
                client_count++;
                printf("Клиент зарегистрирован: %s\n", buffer);
                
            }
            
        }
        pthread_mutex_unlock(&clients_mutex);

        broadcast_message(buffer);
    }

    perror("mq_receive");
    return NULL;
}

void broadcast_message(const char* message) {
    pthread_mutex_lock(&clients_mutex);
    for (int i = 0; i < client_count; i++) {
        if (mq_send(clients[i].mq, message, strlen(message), 0) == -1) {
            perror("mq_send");
        } else {
            printf("Сообщение отправлено клиенту %s: %s\n", clients[i].name, message);
        }
    }
    pthread_mutex_unlock(&clients_mutex);
}

void cleanup() {
    for (int i = 0; i < client_count; i++) {
        mq_close(clients[i].mq);
    }
    mq_unlink(QUEUE_NAME_WRITE);
    mq_unlink(QUEUE_NAME_READ);
}

void sigint_handler(int sig) {
    cleanup();
    exit(EXIT_SUCCESS);
}

int main() {
    mqd_t mq_write, mq_read;
    pthread_t thread;
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 0;
    mq_write = mq_open(QUEUE_NAME_WRITE, O_CREAT | O_WRONLY, 0644, &attr);
    if (mq_write == (mqd_t)-1) {
        perror("mq_open (write)");
        exit(EXIT_FAILURE);
    }
    mq_read = mq_open(QUEUE_NAME_READ, O_CREAT | O_RDONLY, 0644, &attr);
    if (mq_read == (mqd_t)-1) {
        perror("mq_open (read)");
        exit(EXIT_FAILURE);
    }
    if (pthread_create(&thread, NULL, receive_message, &mq_read) != 0) {
        perror("pthread_create");
        exit(EXIT_FAILURE);
    }
    pthread_join(thread, NULL);

    cleanup();
    printf("Сервер завершил работу\n");

    return 0;
}