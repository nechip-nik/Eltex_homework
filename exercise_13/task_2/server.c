#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <pthread.h>
#include <unistd.h>

#define SERVER_QUEUE_NAME "/server_queue"
#define MAX_CLIENTS 10
#define MAX_MSG_SIZE 256
#define CONNECT_PREFIX "CONNECT:"

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
                    //printf("Sent to %s: %s\n", clients[i].name, msg);
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
        if (mq_receive(server_queue, msg, MAX_MSG_SIZE, NULL) == -1)
        {
            continue;
        }
        
        printf("skip\n");
            if (strncmp(msg, CONNECT_PREFIX, strlen(CONNECT_PREFIX)) == 0) {
                // Это сообщение о подключении
                printf("зашел в цикл new client\n");
                char client_name[50];
                strcpy(client_name, msg + strlen(CONNECT_PREFIX));
                printf("New client connected: %s\n", client_name);

                char client_queue_name[50];
                sprintf(client_queue_name, "/client_queue_%s", client_name);

                mqd_t client_queue = mq_open(client_queue_name, O_CREAT | O_RDWR, 0666, &attr);
                if (client_queue == (mqd_t)-1) {
                    perror("mq_open client queue");
                    continue;
                }

                if (client_count < MAX_CLIENTS) {
                    strcpy(clients[client_count].name, client_name);
                    clients[client_count].client_queue = client_queue;
                    client_count++;

                    char join_msg[MAX_MSG_SIZE];
                    sprintf(join_msg, "%s joined the chat", client_name);
                    for (int i = 0; i < client_count; i++) {
                        if (clients[i].client_queue != client_queue) {
                            mq_send(clients[i].client_queue, join_msg, strlen(join_msg) + 1, 0);
                            //printf("Sent to %s: %s\n", clients[i].name, join_msg);
                        }
                    }

                    // if (pthread_create(&threads[client_count - 1], NULL, handle_client, &clients[client_count - 1]) != 0) {
                    //     perror("pthread_create");
                    //     exit(1);
                    // }
                } else {
                    printf("Too many clients\n");
                }
            } else {
                // Это обычное сообщение
                printf("зашел условия обычного сообщения\n");
                printf("%s\n",msg);
                char bullmsg[MAX_MSG_SIZE];
                strcpy(bullmsg, msg);
                char* client_name = strtok(msg, ":");
                printf("%s\n",bullmsg);
                //char* client_msg = strtok(msg, "");

                for (int i = 0; i < client_count; i++) {
                    if (strcmp(clients[i].name, client_name) != 0) {
                        mq_send(clients[i].client_queue, bullmsg, strlen(bullmsg) + 1, 0);
                        //printf("Sent to %s: %s\n", clients[i].name, client_msg);
                    }
                }
            }
    }

    mq_close(server_queue);
    mq_unlink(SERVER_QUEUE_NAME);

    return 0;
}