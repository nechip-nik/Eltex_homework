#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <mqueue.h>
#include <pthread.h>
#include <unistd.h>
#include <ncurses.h>

#define SERVER_QUEUE_NAME "/server_queue"
#define MAX_MSG_SIZE 256
#define CONNECT_PREFIX "CONNECT:"
#define MAX_MESSAGES 100
#define MAX_USERS 10

typedef struct {
    char username[50];
    int online;
} User;

char client_name[50];
mqd_t client_queue;
char messages[MAX_MESSAGES][MAX_MSG_SIZE];
int msg_count = 0;
User users[MAX_USERS];
int user_count = 0;

void* receive_messages(void* arg) {
    char msg[MAX_MSG_SIZE];
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    WINDOW *chat_win = newwin(max_y - 3, max_x - 20, 0, 0);
    WINDOW *users_win = newwin(max_y - 3, 19, 0, max_x - 20);

    while (1) {
        if (mq_receive(client_queue, msg, MAX_MSG_SIZE, NULL) != -1) {
            if (msg_count < MAX_MESSAGES) {
                strcpy(messages[msg_count], msg);
                msg_count++;
            } else {
                for (int i = 0; i < MAX_MESSAGES - 1; i++) {
                    strcpy(messages[i], messages[i + 1]);
                }
                strcpy(messages[MAX_MESSAGES - 1], msg);
            }

            // Обновление списка пользователей
            char* client_name = strtok(msg, ":");
            int found = 0;
            for (int i = 0; i < user_count; i++) {
                if (strcmp(users[i].username, client_name) == 0) {
                    users[i].online = 1;
                    found = 1;
                    break;
                }
            }
            if (!found && user_count < MAX_USERS) {
                strcpy(users[user_count].username, client_name);
                users[user_count].online = 1;
                user_count++;
            }

            // Обновление окон
            wclear(users_win);
            box(users_win, 0, 0);
            for (int i = 0; i < user_count; i++) {
                mvwprintw(users_win, i + 1, 1, "%s %s", users[i].username, users[i].online ? "(online)" : "(offline)");
            }
            wrefresh(users_win);

            wclear(chat_win);
            box(chat_win, 0, 0);
            for (int i = 0; i < msg_count && i < max_y - 4; i++) {
                mvwprintw(chat_win, i + 1, 1, "%s", messages[msg_count - i - 1]);
            }
            wrefresh(chat_win);
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

    // Инициализация ncurses
    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    int max_y, max_x;
    getmaxyx(stdscr, max_y, max_x);
    WINDOW *chat_win = newwin(max_y - 3, max_x - 20, 0, 0);
    WINDOW *users_win = newwin(max_y - 3, 19, 0, max_x - 20);
    WINDOW *input_win = newwin(3, max_x, max_y - 3, 0);

    printw("Enter your name: ");
    refresh();
    scanw("%s", client_name);

    char client_queue_name[50];
    sprintf(client_queue_name, "/client_queue_%s", client_name);

    client_queue = mq_open(client_queue_name, O_CREAT | O_RDWR, 0666, &attr);
    if (client_queue == (mqd_t)-1) {
        perror("mq_open client queue");
        endwin();
        exit(1);
    }

    mqd_t server_queue = mq_open(SERVER_QUEUE_NAME, O_WRONLY);
    if (server_queue == (mqd_t)-1) {
        perror("mq_open server queue");
        endwin();
        exit(1);
    }

    char connect_msg[MAX_MSG_SIZE];
    sprintf(connect_msg, "%s%s", CONNECT_PREFIX, client_name);
    mq_send(server_queue, connect_msg, strlen(connect_msg) + 1, 0);

    pthread_create(&thread, NULL, receive_messages, NULL);

    char msg[MAX_MSG_SIZE];
    while (1) {
        wclear(input_win);
        box(input_win, 0, 0);
        mvwprintw(input_win, 1, 1, "Enter message: ");
        wrefresh(input_win);

        echo();
        wgetnstr(input_win, msg, MAX_MSG_SIZE - 1);
        noecho();

        char full_msg[MAX_MSG_SIZE];
        sprintf(full_msg, "%s: %s", client_name, msg);

        for (int i = 0; i < strlen(full_msg); i++) {
            if (full_msg[i] == '\n') {
                full_msg[i] = ' ';
            }
        }
        mq_send(server_queue, full_msg, strlen(full_msg) + 1, 0);
    }

    mq_close(client_queue);
    mq_unlink(client_queue_name);

    endwin();
    return 0;
}