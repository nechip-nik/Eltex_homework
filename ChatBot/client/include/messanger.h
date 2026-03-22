#ifndef MESSANGER_H
#define MESSANGER_H

#include <arpa/inet.h>
#include <errno.h>
#include <ncurses.h>
#include <pthread.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <time.h>
#include <unistd.h>

#define SERVER_IP "127.0.0.1" // IP-адрес сервера
#define SERVER_PORT 34543     // Порт сервера

extern int sockfd;

#define MAX_NAME_LEN 32
#define MAX_MESSAGE_LEN 1024
#define MAX_CLIENTS 100
#define MAX_MESSAGES 1000
#define MAX_DATE_LEN 64

typedef struct
{
    char username[MAX_NAME_LEN];
    char message[MAX_MESSAGE_LEN];
} Message;

typedef struct
{
    int num_clients;
    char clients[MAX_CLIENTS][MAX_NAME_LEN];
    Message messages[MAX_MESSAGES];
    int message_count;
} ChatRoom;

extern int key_catcher[3];

extern ChatRoom chatroom;

extern char current_user[MAX_NAME_LEN];
extern char input_message[MAX_MESSAGE_LEN];

extern WINDOW *message_win, *user_win, *input_win;
extern WINDOW *message_win_border, *user_win_border, *input_win_border;
extern int message_win_height, message_win_width;
extern int user_win_height, user_win_width;
extern int input_win_height, input_win_width;

extern int message_scroll_pos;
extern int user_scroll_pos;

extern pthread_t receive_thread;

extern char login[MAX_NAME_LEN];
extern char password[MAX_NAME_LEN];

void run_client();
void init_ncurses();
void create_windows();
void display_messages();
void display_input();
void handle_input();
void clean_up();
void *receive_messages(void *arg);
void add_message(const char *username, const char *message);
int login_or_register_window();
void registration_window();
void login_window();

int send_login(const char *username, const char *password);
void *receive_from_server(void *arg);
int send_registration(const char *username, const char *password);
void send_message_to_server(const char *message);
int connect_to_server();

#endif // MESSANGER_H