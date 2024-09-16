#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>

#define MSG_SIZE 128

struct msg_buffer {
    long msg_type;
    char msg_text[MSG_SIZE];
};

int main() {
    key_t key;
    int msgid;
    struct msg_buffer message;

    key = ftok("server", 65);
    if (key == -1) {
        perror("ftok");
        exit(EXIT_FAILURE);
    }

    msgid = msgget(key, 0666 | IPC_CREAT);
    if (msgid == -1) {
        perror("msgget");
        exit(EXIT_FAILURE);
    }

    message.msg_type = 1;
    strcpy(message.msg_text, "Hi!");

    if (msgsnd(msgid, &message, sizeof(message.msg_text), 0) == -1) {
        perror("msgsnd");
        exit(EXIT_FAILURE);
    }

    printf("Сервер отправил сообщение\n");

    if (msgrcv(msgid, &message, sizeof(message.msg_text), 2, 0) == -1) {
        perror("msgrcv");
        exit(EXIT_FAILURE);
    }

    printf("Сервер получил сообщение: '%s'\n", message.msg_text);

    if (msgctl(msgid, IPC_RMID, NULL) == -1) {
        perror("msgctl");
        exit(EXIT_FAILURE);
    }

    printf("Сервер закрыл очередь\n");

    return 0;
}