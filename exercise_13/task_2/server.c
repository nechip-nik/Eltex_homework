#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <pthread.h>
#include <unistd.h>

#define SERVER_QUEUE_NAME "/server_queue"
#define MAX_CLIENTS 10
#define MAX_MSG_SIZE 256

typedef struct {
    mqd_t client_queue;
    char name[50];
} ClientInfo;

ClientInfo clients[MAX_CLIENTS];
int client_count = 0;

void* handle_client(void* arg) {
    ClientInfo* client = (ClientInfo*)arg;
    char msg[MAX_MSG_SIZE];

    while (1) {
        if (mq_receive(client->client_queue, msg, MAX_MSG_SIZE, NULL) != -1) {
            for (int i = 0; i < client_count; i++) {
                if (clients[i].client_queue != client->client_queue) {
                    mq_send(clients[i].client_queue, msg, strlen(msg) + 1, 0);
                    printf("print to %i message %s\n", i, msg);
                }
            }
        }
    }

    return NULL;
}

int main() {
    mqd_t server_queue;
    struct mq_attr attr;
    pthread_t threads[MAX_CLIENTS];

    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    server_queue = mq_open(SERVER_QUEUE_NAME, O_CREAT | O_RDONLY, 0666, &attr);
    if (server_queue == (mqd_t)-1) {
        perror("mq_open");
        exit(1);
    }

    char msg[MAX_MSG_SIZE];
    while (1) {
        if (mq_receive(server_queue, msg, MAX_MSG_SIZE, NULL) != -1) {
            printf("New client connected: %s\n", msg);

            char client_queue_name[50];
            sprintf(client_queue_name, "/client_queue_%s", msg);

            mqd_t client_queue = mq_open(client_queue_name, O_CREAT | O_RDWR, 0666, &attr);
            if (client_queue == (mqd_t)-1) {
                perror("mq_open client queue");
                continue;
            }

            if (client_count < MAX_CLIENTS) {
                strcpy(clients[client_count].name, msg);
                clients[client_count].client_queue = client_queue;
                client_count++;

                char join_msg[MAX_MSG_SIZE];
                sprintf(join_msg, "%s joined the chat", msg);
                for (int i = 0; i < client_count; i++) {
                    mq_send(clients[i].client_queue, join_msg, strlen(join_msg) + 1, 0);
                }

                pthread_create(&threads[client_count - 1], NULL, handle_client, &clients[client_count - 1]);
            } else {
                printf("Too many clients\n");
            }
        }
    }

    mq_close(server_queue);
    mq_unlink(SERVER_QUEUE_NAME);

    return 0;
}