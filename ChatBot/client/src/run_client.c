#include "../include/messanger.h"

ChatRoom chatroom;
Message messages[MAX_MESSAGES];
char current_user[MAX_NAME_LEN];
char input_message[MAX_MESSAGE_LEN];
char login[MAX_NAME_LEN];
char password[MAX_NAME_LEN];
int key_catcher[3];

WINDOW *message_win, *user_win, *input_win;
WINDOW *message_win_border, *user_win_border, *input_win_border;
int message_win_height, message_win_width;
int user_win_height, user_win_width;
int input_win_height, input_win_width;

int message_scroll_pos;
int user_scroll_pos;
int active_window;

pthread_t receive_thread;
int sockfd;

int connect_to_server()
{
    struct sockaddr_in server_addr;

    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0)
    {
        show_temp_window("Socket creation failed");
        return -1;
    }

    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(SERVER_PORT);

    if (inet_pton(AF_INET, SERVER_IP, &server_addr.sin_addr) <= 0)
    {
        show_temp_window("Invalid server IP address");
        close(sockfd);
        return -1;
    }

    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) <
        0)
    {
        show_temp_window("Connection to server failed");
        close(sockfd);
        return -1;
    }

    return 0;
}

void *receive_from_server(void *arg)
{
    char buffer[MAX_MESSAGE_LEN + MAX_NAME_LEN + 32];

    while (1)
    {
        int len = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
        if (len <= 0)
        {
            if (len < 0)
                show_temp_window("receiving data, empty response");
            break;
        }

        buffer[len] = '\0';
        char type[MAX_NAME_LEN], sender[MAX_NAME_LEN], message[MAX_MESSAGE_LEN];

        // Парсим тип сообщения, отправителя и сам текст сообщения
        if (sscanf(buffer, "%s %s %[^\n]", type, sender, message) == 3)
        {
            if (strncmp(type, "MESSAGE", 7) == 0)
            {
                // Если это обычное сообщение, выводим его в обычном цвете
                add_message(sender, message);
                display_messages();
            }
            else if (strncmp(type, "ERROR", 5) == 0)
            {
                // Если это ошибка, выводим красным текстом
                wattron(message_win, COLOR_PAIR(2)); // Включаем красный цвет
                add_message(sender, message);
                wattroff(message_win, COLOR_PAIR(2)); // Выключаем красный цвет
                display_messages();
            }
            else if (strncmp(type, "BAN", 3) == 0)
            {
                // Если это бан, выводим сообщение в окно ошибок
                show_temp_window(message);
                clean_up();
                exit(0);
            }
        }
    }

    return NULL;
}

void send_message_to_server(const char *message)
{
    char buffer[MAX_MESSAGE_LEN + MAX_NAME_LEN + 16];
    snprintf(buffer, sizeof(buffer), "MESSAGE %s %s\n", current_user, message);
    if (send(sockfd, buffer, strlen(buffer), 0) < 0)
    {
        show_temp_window("send message");
    }
}

int send_registration(const char *username, const char *password)
{
    char buffer[MAX_NAME_LEN * 2 + 16];
    snprintf(buffer, sizeof(buffer), "REGISTER %s %s\n", username, password);

    // Отправляем запрос на сервер
    if (send(sockfd, buffer, strlen(buffer), 0) < 0)
    {
        show_temp_window("send registration data");
        return -1;
    }

    // Ожидаем только один ответ от сервера
    char response[256];
    int len = recv(sockfd, response, sizeof(response) - 1, 0);
    if (len <= 0)
    {
        show_temp_window("to receive registration response");
        return -1;
    }

    response[len] = '\0';
    if (strncmp(response, "Registration successful\n", 25) == 0)
    {
        return 0;
    }
    else
    {
        show_temp_window(response);
        return -1;
    }
}

int send_login(const char *username, const char *password)
{
    char buffer[MAX_NAME_LEN * 2 + 16];
    snprintf(buffer, sizeof(buffer), "LOGIN %s %s\n", username, password);

    // Отправляем запрос на сервер
    if (send(sockfd, buffer, strlen(buffer), 0) < 0)
    {
        show_temp_window("send login data");
        return -1;
    }

    // Ожидаем только один ответ от сервера
    char response[256];
    int len = recv(sockfd, response, sizeof(response) - 1, 0);
    if (len <= 0)
    {
        show_temp_window("receive login response");
        return -1;
    }

    response[len] = '\0';
    if (strncmp(response, "Login successful\n", 18) == 0)
    {
        return 0;
    }
    else
    {
        show_temp_window(response);
        return -1;
    }
}

void show_temp_window(const char *text)
{

    start_color();

    WINDOW *win = newwin(9, 50, LINES / 3, COLS / 3);

    wattron(win, COLOR_PAIR(6));
    box(win, 0, 0);
    wattroff(win, COLOR_PAIR(6));

    wattron(win, COLOR_PAIR(6));
    mvwprintw(win, 4, 2, "Failed: %s", text);
    wattroff(win, COLOR_PAIR(6));
    wrefresh(win);

    napms(2000);

    wclear(win);
    wrefresh(win);
    delwin(win);
}

void run_client()
{

    signal(SIGINT, clean_up);
    // Попытка подключения к серверу
    if (connect_to_server() < 0)
    {
        fprintf(stderr, "  Failed to connect to server\n");
        exit(EXIT_FAILURE);
    }

    // Инициализация ncurses
    init_ncurses();

    int auth_success = 0; // Флаг успешной авторизации
    while (!auth_success)
    {
        int choice =
            login_or_register_window(""); // Получаем выбор пользователя

        if (choice == 1)
        { // Регистрация
            registration_window();
            int reg_status = send_registration(login, password);
            if (reg_status == 0)
            {
                auth_success = 1; // Успешная регистрация
            }
        }
        else if (choice == 2)
        { // Логин
            login_window();
            int login_status = send_login(login, password);
            if (login_status == 0)
            {
                auth_success = 1; // Успешный логин
            }
        }
    }

    // Сохранение текущего пользователя
    strncpy(current_user, login, MAX_NAME_LEN);

    // Переход к основному окну чата
    create_windows();
    display_messages();
    display_input();

    // Запуск потока для приема сообщений
    pthread_create(&receive_thread, NULL, receive_from_server, NULL);

    // Основной цикл обработки пользовательского ввода
    while (1)
    {
        handle_input();
    }
}