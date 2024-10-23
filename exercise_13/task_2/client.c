#include <mqueue.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <pthread.h>
#include <signal.h>
#include <ncurses.h>

#define QUEUE_NAME_READ "/queue_write"
#define QUEUE_NAME_WRITE "/queue_read"
#define MAX_SIZE 1024
#define MAX_USERS 100

char nickname[256];
char sender_nickname[256]; 
char users[MAX_USERS][256]; 
int user_count = 0;

WINDOW *chat_win, *users_win;

// Функция, которая будет выполняться в отдельном потоке для чтения сообщений
void* receive_message(void* arg) {
    mqd_t mq_read = *(mqd_t*)arg;
    char buffer[MAX_SIZE];
    ssize_t bytes_read;

    while ((bytes_read = mq_receive(mq_read, buffer, MAX_SIZE, NULL)) != -1) {
        buffer[bytes_read] = '\0';
        char* colon_pos = strchr(buffer, ':');
        if (colon_pos != NULL) {
            *colon_pos = '\0'; 
            strncpy(sender_nickname, buffer, sizeof(sender_nickname) - 1);
            sender_nickname[sizeof(sender_nickname) - 1] = '\0';
            int found = 0;
            for (int i = 0; i < user_count; i++) {
                if (strcmp(users[i], sender_nickname) == 0) {
                    found = 1;
                    break;
                }
            }
            if (!found) {
                strncpy(users[user_count], sender_nickname, sizeof(users[user_count]) - 1);
                users[user_count][sizeof(users[user_count]) - 1] = '\0';
                user_count++;
                wclear(users_win);
                box(users_win, 0, 0);
                for (int i = 0; i < user_count; i++) {
                    mvwprintw(users_win, i + 1, 1, "%s", users[i]);
                }
                wrefresh(users_win);
            }
            mvwprintw(chat_win, LINES - 3, 0, "%s: %s", sender_nickname, colon_pos + 2); 
            wscrl(chat_win, 1); 
            wrefresh(chat_win);
        } else {
            mvwprintw(chat_win, LINES - 3, 0, "new connection");
            wscrl(chat_win, 1);
            wrefresh(chat_win);
        }
    }

    perror("mq_receive");
    return NULL;
}

// Функция, которая будет выполняться в отдельном потоке для отправки сообщений
void* send_message(void* arg) {
    mqd_t mq_write = *(mqd_t*)arg;
    char response[MAX_SIZE];
    int pos = 0;

    while (1) {
        mvwaddstr(chat_win, LINES - 1, 0, "> ");
        wrefresh(chat_win);

        int ch = getch();
        if (ch == '\n') {
            response[pos] = '\0';
            pos = 0;

            if (strcmp(response, "exit") == 0) {
                break;
            }

            char message[MAX_SIZE];
            snprintf(message, MAX_SIZE, "%s : %s", nickname, response);

            if (mq_send(mq_write, message, strlen(message), 0) == -1) {
                perror("mq_send");
                exit(EXIT_FAILURE);
            }

            mvwprintw(chat_win, LINES - 1, 0, "                                        ");
        } else if (ch == KEY_BACKSPACE || ch == 127) {
            if (pos > 0) {
                pos--;
                mvwaddch(chat_win, LINES - 1, pos + 2, ' ');
                wmove(chat_win, LINES - 1, pos + 2);
                wrefresh(chat_win);
            }
        } else {
            if (pos < MAX_SIZE - 1) {
                response[pos++] = ch;
                mvwaddch(chat_win, LINES - 1, pos + 2, ch);
                wrefresh(chat_win);
            }
        }
    }

    return NULL;
}

void cleanup(mqd_t mq_write, mqd_t mq_read) {
    mq_close(mq_write);
    mq_close(mq_read);
}

void sigint_handler(int sig) {
    endwin(); 
    exit(EXIT_SUCCESS);
}

int main() {
    mqd_t mq_write, mq_read;
    pthread_t recv_thread, send_thread;
    struct mq_attr attr;
    attr.mq_flags = 0;
    attr.mq_maxmsg = 10;
    attr.mq_msgsize = MAX_SIZE;
    attr.mq_curmsgs = 0;

    char client_queue_name[256];
    snprintf(client_queue_name, sizeof(client_queue_name), "/client_queue_%d", getpid());

    mq_read = mq_open(client_queue_name, O_CREAT | O_RDONLY, 0644, &attr);
    if (mq_read == (mqd_t)-1) {
        perror("mq_open (read)");
        exit(EXIT_FAILURE);
    }

    mq_write = mq_open(QUEUE_NAME_WRITE, O_WRONLY);
    if (mq_write == (mqd_t)-1) {
        perror("mq_open (write)");
        exit(EXIT_FAILURE);
    }

    initscr();
    cbreak();
    noecho();
    keypad(stdscr, TRUE);
    scrollok(stdscr, TRUE);

    // Создаем окно чата
    chat_win = newwin(LINES - 1, COLS - 20, 0, 0);
    scrollok(chat_win, TRUE);
    wrefresh(chat_win);

    // Создаем окно пользователей
    users_win = newwin(LINES - 1, 20, 0, COLS - 20);
    box(users_win, 0, 0);
    mvwprintw(users_win, 0, 1, "Users");
    wrefresh(users_win);

    mvprintw(0, 0, "Enter your nickname: ");
    refresh();
    getstr(nickname);
    nickname[strcspn(nickname, "\n")] = '\0';


    if (mq_send(mq_write, client_queue_name, strlen(client_queue_name), 0) == -1) {
        perror("mq_send (register)");
        exit(EXIT_FAILURE);
    }
    if (pthread_create(&recv_thread, NULL, receive_message, &mq_read) != 0) {
        perror("pthread_create (receive)");
        exit(EXIT_FAILURE);
    }
    if (pthread_create(&send_thread, NULL, send_message, &mq_write) != 0) {
        perror("pthread_create (send)");
        exit(EXIT_FAILURE);
    }


    pthread_join(recv_thread, NULL);
    pthread_join(send_thread, NULL);

    cleanup(mq_write, mq_read);
    mq_unlink(client_queue_name);

    delwin(chat_win);
    delwin(users_win);
    endwin(); 

    return 0;
}