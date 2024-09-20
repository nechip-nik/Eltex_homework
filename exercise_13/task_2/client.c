#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <pthread.h>
#include <unistd.h>

#define SERVER_QUEUE_NAME "/server_queue"
#define MAX_MSG_SIZE 256

char client_name[50];
mqd_t client_queue;

void* receive_messages(void* arg) {
    char msg[MAX_MSG_SIZE];

    while (1) {
        if (mq_receive(client_queue, msg, MAX_MSG_SIZE, NULL) != -1) {
            printf("%s\n", msg);
        }
    }

    return NULL;
}

int main() {
    struct mq_attr attr;
    pthread_t thread;

    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_MSG_SIZE;
    attr.mq_curmsgs = 0;

    printf("Enter your name: ");
    scanf("%s", client_name);

    // Create client queue
    char client_queue_name[50];
    sprintf(client_queue_name, "/client_queue_%s", client_name);

    client_queue = mq_open(client_queue_name, O_CREAT | O_RDWR, 0666, &attr);
    if (client_queue == (mqd_t)-1) {
        perror("mq_open client queue");
        exit(1);
    }

    // Connect to server
    mqd_t server_queue = mq_open(SERVER_QUEUE_NAME, O_WRONLY);
    if (server_queue == (mqd_t)-1) {
        perror("mq_open server queue");
        exit(1);
    }

    // Send client name to server
    mq_send(server_queue, client_name, strlen(client_name) + 1, 0);

    // Start thread to receive messages
    pthread_create(&thread, NULL, receive_messages, NULL);

    // Send messages to server
    char msg[MAX_MSG_SIZE];
    while (1) {
        printf("Enter message: ");
        scanf(" %[^\n]", msg);

        char full_msg[MAX_MSG_SIZE];
        sprintf(full_msg, "%s: %s", client_name, msg);

        for (int i = 0; i < strlen(full_msg); i++) {
            if (full_msg[i] == '\n') {
                full_msg[i] = ' ';
            }
        }

        mq_send(client_queue, full_msg, strlen(full_msg) + 1, 0);
    }

    mq_close(client_queue);
    mq_unlink(client_queue_name);

    return 0;
}