#include "../include/lib_client_handler.h"
#include "../include/datafileparser.h"
#include "../include/lib_user.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

User clients[MAX_COUNT_USERS];
int client_count = 0;
pthread_mutex_t clients_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t user_id_mutex = PTHREAD_MUTEX_INITIALIZER;

#define MAX_JOKES 100
#define MAX_QUOTES 100

struct JOKE_QUOTE jokes[MAX_JOKES];
struct JOKE_QUOTE quotes[MAX_QUOTES];

int last_user_id = 0;
int joke_count = 0;
int quote_count = 0;
pthread_mutex_t jokes_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t quotes_mutex = PTHREAD_MUTEX_INITIALIZER;

void add_joke(int user_id, const char *type, const char *content)
{
    pthread_mutex_lock(&jokes_mutex); // Блокируем доступ к массиву шуток

    if (joke_count < MAX_JOKES)
    {
        // Добавляем новую шутку в массив
        jokes[joke_count].user_id = user_id;
        strncpy(jokes[joke_count].type, type,
                sizeof(jokes[joke_count].type) - 1);
        strncpy(jokes[joke_count].content, content,
                sizeof(jokes[joke_count].content) - 1);
        jokes[joke_count].type[sizeof(jokes[joke_count].type) - 1] =
            '\0'; // Гарантируем завершение строки
        jokes[joke_count].content[sizeof(jokes[joke_count].content) - 1] =
            '\0'; // Гарантируем завершение строки
        joke_count++;

        // Выводим всю структуру шуток в терминал
        printf("Current jokes in the array:\n");
        for (int i = 0; i < joke_count; i++)
        {
            printf("Joke %d:\n", i + 1);
            printf("  User ID: %d\n", jokes[i].user_id);
            printf("  Type: %s\n", jokes[i].type);
            printf("  Content: %s\n", jokes[i].content);
            printf("----------------------------\n");
        }
    }
    else
    {
        printf(
            "Error: Maximum number of jokes reached (%d). Cannot add more.\n",
            MAX_JOKES);
    }

    pthread_mutex_unlock(&jokes_mutex); // Разблокируем доступ к массиву шуток
}
void add_quote(int user_id, const char *type, const char *content)
{
    pthread_mutex_lock(&quotes_mutex);
    if (quote_count < MAX_QUOTES)
    {
        quotes[quote_count].user_id = user_id;
        strncpy(quotes[quote_count].type, type,
                sizeof(quotes[quote_count].type) - 1);
        strncpy(quotes[quote_count].content, content,
                sizeof(quotes[quote_count].content) - 1);
        quotes[quote_count].type[sizeof(quotes[quote_count].type) - 1] = '\0';
        quotes[quote_count].content[sizeof(quotes[quote_count].content) - 1] =
            '\0';
        quote_count++;
    }
    pthread_mutex_unlock(&quotes_mutex);
}

void *handle_client(void *arg)
{
    User *client = (User *)arg;

    char buffer[BUFFER_SIZE];
    char filename[MAX_NAME_LENGTH + 4];
    struct sockaddr_in client_addr;
    socklen_t addr_len = sizeof(client_addr);

    // Извлечение IP-адреса клиента
    if (getpeername(client->socket, (struct sockaddr *)&client_addr,
                    &addr_len) == 0)
    {
        inet_ntop(AF_INET, &client_addr.sin_addr, client->ip, INET_ADDRSTRLEN);
        printf("Client connected from: %s\n", client->ip);
    }
    else
    {
        strncpy(client->ip, "unknown", INET_ADDRSTRLEN);
    }

    // Проверка на бан по IP
    if (is_ip_banned(client->ip))
    {
        send_message(client->socket,
                     "MESSAGE filmbot You are banned from this server.\n");
        close(client->socket);
        return NULL;
    }

    while (1)
    {
        memset(buffer, 0, BUFFER_SIZE);
        int bytes_received = recv(client->socket, buffer, BUFFER_SIZE - 1, 0);

        if (bytes_received <= 0)
        {
            printf("Client disconnected: %s\n", client->name);
            close(client->socket);

            pthread_mutex_lock(&clients_mutex);
            for (int i = 0; i < client_count; i++)
            {
                if (clients[i].socket == client->socket)
                {
                    for (int j = i; j < client_count - 1; j++)
                    {
                        clients[j] = clients[j + 1];
                    }
                    client_count--;
                    break;
                }
            }
            pthread_mutex_unlock(&clients_mutex);

            break;
        }

        buffer[bytes_received] = '\0';
        printf("Received message from %s: %s\n", client->name, buffer);
        // Обработка команды /exit
        if (strncmp(buffer, "/exit", 5) == 0)
        {
            printf("Client %s requested to exit.\n", client->name);
            send_message(client->socket, "Goodbye!\n");
            write_to_file(client->name, "Goodbye!");
            close(client->socket);

            pthread_mutex_lock(&clients_mutex);
            for (int i = 0; i < client_count; i++)
            {
                if (clients[i].socket == client->socket)
                {
                    for (int j = i; j < client_count - 1; j++)
                    {
                        clients[j] = clients[j + 1];
                    }
                    clients[i].active = ACTION_ANONYMOUS;
                    client_count--;
                    break;
                }
            }
            pthread_mutex_unlock(&clients_mutex);

            break;
        }

        // Если пользователь не зарегистрирован, обрабатываем только LOGIN и
        // REGISTER
        if (client->active == ACTION_ANONYMOUS)
        {
            if (strncmp(buffer, "LOGIN", 5) == 0)
            {
                char username[32], password[32];
                if (sscanf(buffer + 6, "%s %s", username, password) == 2)
                {
                    UserErrorCode result = login_user(username, password);
                    switch (result)
                    {
                    case USER_SUCCESS:
                        strncpy(client->name, username, MAX_NAME_LENGTH - 1);
                        client->name[MAX_NAME_LENGTH - 1] = '\0';
                        strncpy(client->password, password,
                                MAX_PASSWORD_LENGTH - 1);
                        client->password[MAX_PASSWORD_LENGTH - 1] = '\0';
                        client->active = ACTION_LOGGED_IN;
                        char filename[MAX_NAME_LENGTH + 4];
                        char chat_history_buffer[1024];
                        send_message(client->socket, "Login successful\n");
                        snprintf(filename, sizeof(filename), "history/%s.txt",
                                 username);
                        FILE *file = fopen(filename, "r");
                        if (file != NULL)
                        {
                            // исория чата
                            while (fgets(buffer, sizeof(chat_history_buffer),
                                         file) != NULL)
                            {
                                printf("%s", buffer);
                                sleep(1);
                                send_message(client->socket, buffer);
                            }
                            fclose(file);
                        }
                        break;
                    case USER_ERROR_BANNED:
                        send_message(client->socket,
                                     "ERROR FilmBot User is banned\n");
                        break;
                    case USER_ERROR_NOT_FOUND:
                        send_message(client->socket,
                                     "ERROR FilmBot User not found\n");
                        break;
                    case USER_ERROR_INVALID_INPUT:
                        send_message(client->socket,
                                     "ERROR FilmBot Invalid input\n");
                        break;
                    default:
                        send_message(client->socket,
                                     "ERROR FilmBot Unknown error\n");
                        break;
                    }
                }
                else
                {
                    send_message(client->socket,
                                 "ERROR FilmBot Invalid LOGIN command\n");
                }
            }
            else if (strncmp(buffer, "REGISTER", 8) == 0)
            {
                char username[32], password[32];
                if (sscanf(buffer + 9, "%s %s", username, password) == 2)
                {
                    UserErrorCode result =
                        registration_user(username, password);
                    switch (result)
                    {
                    case USER_SUCCESS:
                    pthread_mutex_lock(&user_id_mutex); // Блокируем доступ к last_user_id
            last_user_id++; // Увеличиваем ID
            client->id = last_user_id; // Присваиваем новый ID пользователю
            pthread_mutex_unlock(&user_id_mutex);
                        strncpy(client->name, username, MAX_NAME_LENGTH - 1);
                        client->name[MAX_NAME_LENGTH - 1] = '\0';
                        strncpy(client->password, password,
                                MAX_PASSWORD_LENGTH - 1);
                        client->password[MAX_PASSWORD_LENGTH - 1] = '\0';
                        client->active = ACTION_REGISTERED;
                        send_message(client->socket,
                                     "Registration successful\n");
                        write_to_file(client->name, "Registration successful");
                        char filename[MAX_NAME_LENGTH +
                                      4]; // +4 для ".txt" и нулевого символа
                        snprintf(filename, sizeof(filename), "history/%s.txt",
                                 username);
                        FILE *file = fopen(filename, "w");
                        if (file != NULL)
                        {
                            fclose(file);
                        }
                        break;
                    case USER_ERROR_EXISTS:
                        send_message(client->socket,
                                     "ERROR FilmBot Username already exists\n");
                        break;
                    case USER_ERROR_MAX_USERS:
                        send_message(client->socket,
                                     "ERROR FilmBot Maximum users reached\n");
                        break;
                    case USER_ERROR_SHORT_LOGIN:
                        send_message(client->socket,
                                     "ERROR FilmBot Login is too short\n");
                        break;
                    case USER_ERROR_INVALID_LOGIN:
                        send_message(client->socket,
                                     "ERROR FilmBot Login contains special "
                                     "characters\n");
                        break;
                    case USER_ERROR_SHORT_PASSWORD:
                        send_message(client->socket,
                                     "ERROR FilmBot Password is too short\n");
                        break;
                    case USER_ERROR_EMPTY_LOGIN:
                        send_message(client->socket,
                                     "ERROR FilmBot Login is empty\n");
                        break;
                    case USER_ERROR_EMPTY_PASSWORD:
                        send_message(client->socket,
                                     "ERROR FilmBot Password is empty\n");
                        break;
                    case USER_ERROR_INVALID_INPUT:
                        send_message(client->socket,
                                     "ERROR FilmBot Invalid input\n");
                        break;
                    default:
                        send_message(client->socket,
                                     "ERROR FilmBot Unknown error\n");
                        break;
                    }
                }
                else
                {
                    send_message(client->socket,
                                 "ERROR FilmBot Invalid REGISTER command\n");
                }
            }
            else
            {
                send_message(
                    client->socket,
                    "ERROR FilmBot You must register or login first.\n");
            }
        }
        else if (strncmp(buffer, "MESSAGE", 7) == 0 &&
                 (client->active == ACTION_REGISTERED ||
                  client->active == ACTION_LOGGED_IN))
        {
            snprintf(filename, sizeof(filename), "history/%s.txt",
                     client->name);
            FILE *file = fopen(filename, "a");
            if (file != NULL)
            {
                fprintf(file, "%s", buffer);
                fclose(file);
            }

            char *message_content = buffer + 7; // Пропускаем "MESSAGE"
            message_content += strlen(client->name) +
                               2; // Пропускаем имя пользователя и пробел

            printf("Message content:%s", message_content);

            if (strncmp(message_content, "/help", 5) == 0)
            {
                client->count_ban =
                    0; // Reset ban count (assuming this is part of your logic)
                send_help_info_file(
                    client->name, client->socket,
                    "server/data/help.txt"); // Call the function to handle the
                                             // file
            }
            else if (strncmp(message_content, "/info", 5) == 0)
            {
                client->count_ban =
                    0; // Reset ban count (assuming this is part of your logic)
                send_help_info_file(
                    client->name, client->socket,
                    "server/data/info.txt"); // Call the function to handle the
                                             // file
            }
            else if (strncmp(message_content, "/quote", 6) == 0)
            {
                // Обработка команды /quote
                client->count_ban = 0;
                char type[20];
                if (sscanf(message_content + 6, "%s", type) == 1)
                {
                    printf("Received type: %s\n", type); // Для отладки

                    // Используем strcmp для сравнения строк
                    if (strcmp(type, "literary") == 0 ||
                        strcmp(type, "scientific") == 0)
                    {
                        printf("Valid type: %s\n", type); // Для отладки
                        search_quotes(client->name, client->socket, type,
                                      client->id, quotes, quote_count);
                    }
                    else
                    {
                        send_message(client->socket,
                                     "ERROR FilmBot Invalid quote type. Use "
                                     "'literary' or 'scientific'.\n");
                        write_to_file(client->name,
                                      "Invalid quote type. Use 'literary' or "
                                      "'scientific'.");
                    }
                }
                else
                {
                    send_message(client->socket,
                                 "ERROR FilmBot Invalid /quote command format. "
                                 "Use /quote [literary/scientific].\n");
                    write_to_file(client->name, "Invalid /name command format. "
                                                "Use /name [movie name].");
                }
            }
            else if (strncmp(message_content, "/new_quote", 10) == 0)
            {
                client->count_ban = 0;
                char type[20];
                char text[256];
                if (sscanf(message_content + 11, "%s %[^\n]", type, text) == 2)
                {
                    if (strcmp(type, "literary") == 0 ||
                        strcmp(type, "scientific") == 0)
                    {
                        add_quote(client->id, type, text);
                        send_message(
                            client->socket,
                            "MESSAGE FilmBot Quote added successfully.\n");
                        write_to_file(client->name, "Quote added successfully");
                    }
                    else
                    {
                        send_message(client->socket,
                                     "ERROR FilmBot Invalid quote type. Use "
                                     "'literary' or 'scientific'.\n");
                        write_to_file(client->name,
                                      "Invalid quote type. Use 'literary' or "
                                      "'scientific'.");
                    }
                }
                else
                {
                    send_message(
                        client->socket,
                        "ERROR FilmBot Invalid /new_quote command format. Use "
                        "/new_quote [literary/scientific] [text].\n");
                    write_to_file(client->name,
                                  "Invalid /new_quote command format. Use "
                                  "/new_quote [literary/scientific] [text]");
                }
            }
            else if (strncmp(message_content, "/joke", 5) == 0)
            {
                client->count_ban = 0;
                char type[20];
                if (sscanf(message_content + 6, "%s", type) == 1)
                {
                    printf("Received type: %s\n", type); // Для отладки

                    // Используем strcmp для сравнения строк
                    if (strcmp(type, "funny") == 0 ||
                        strcmp(type, "unfunny") == 0)
                    {
                        printf("Valid type: %s\n", type); // Для отладки
                        search_jokes(client->name, client->socket, type,
                                     client->id, jokes, joke_count);
                    }
                    else
                    {
                        send_message(client->socket,
                                     "ERROR FilmBot Invalid joke type. Use "
                                     "'funny' or 'unfunny'.\n");
                        write_to_file(
                            client->name,
                            "Invalid joke type. Use 'funny' or 'unfunny'.");
                    }
                }
                else
                {
                    send_message(client->socket,
                                 "ERROR FilmBot Invalid /joke command format. "
                                 "Use /joke [funny/unfunny].\n");
                    write_to_file(client->name, "Invalid /joke command format. "
                                                "Use /joke [funny/unfunny].");
                }
            }
            else if (strncmp(message_content, "/new_joke", 9) == 0)
            {
                client->count_ban = 0;
                char type[20];
                char text[256];
                if (sscanf(message_content + 10, "%s %[^\n]", type, text) == 2)
                {
                    if (strcmp(type, "funny") == 0 ||
                        strcmp(type, "unfunny") == 0)
                    {
                        add_joke(client->id, type, text);
                        send_message(
                            client->socket,
                            "MESSAGE FilmBot Joke added successfully.\n");
                        write_to_file(client->name, "Joke added successfully.");
                    }
                    else
                    {
                        send_message(client->socket,
                                     "ERROR FilmBot Invalid joke type. Use "
                                     "'funny' or 'unfunny'.\n");
                        write_to_file(
                            client->name,
                            "Invalid joke type. Use 'funny' or 'unfunny'.");
                    }
                }
                else
                {
                    send_message(
                        client->socket,
                        "ERROR FilmBot Invalid /new_joke command format. Use "
                        "/new_joke [funny/unfunny] [text].\n");
                    write_to_file(client->name,
                                  "Invalid /new_joke command format. Use "
                                  "/new_joke [funny/unfunny] [text].");
                }
            }
            else if (strncmp(message_content, "/name", 5) == 0)
            {
                client->count_ban = 0;
                char name[256];
                if (sscanf(message_content + 6, "%[^\n]", name) == 1)
                {
                    printf("%s\n", name);
                    search_films(client->name, client->socket, name,
                                 SEARCH_BY_NAME);
                }
                else
                {
                    send_message(client->socket,
                                 "ERROR FilmBot Invalid /name command format. "
                                 "Use /name [movie name].\n");
                    write_to_file(client->name, "Invalid /name command format. "
                                                "Use /name [movie name].");
                }
            }
            else if (strncmp(message_content, "/genre", 6) == 0)
            {
                client->count_ban = 0;
                char genre[256];
                if (sscanf(message_content + 7, "%[^\n]", genre) == 1)
                {
                    search_films(client->name, client->socket, genre,
                                 SEARCH_BY_GENRE);
                }
                else
                {
                    send_message(client->socket,
                                 "ERROR FilmBot Invalid /genre command format. "
                                 "Use /genre [genre].\n");
                    write_to_file(
                        client->name,
                        "Invalid /genre command format. Use /genre [genre].");
                }
            }
            else if (strncmp(message_content, "/year", 5) == 0)
            {
                client->count_ban = 0;
                char year[12];
                if (sscanf(message_content + 6, "%s", year) == 1)
                {
                    printf("%s\n", year);
                    search_films(client->name, client->socket, year,
                                 SEARCH_BY_YEAR);
                }
                else
                {
                    send_message(client->socket,
                                 "ERROR FilmBot Invalid /year command format. "
                                 "Use /year [year].\n");
                    write_to_file(
                        client->name,
                        "Invalid /year command format. Use /year [year].");
                }
            }
            else if (strncmp(message_content, "/director", 9) == 0)
            {
                client->count_ban = 0;
                char director[256];
                if (sscanf(message_content + 10, "%[^\n]", director) == 1)
                {

                    search_films(client->name, client->socket, director,
                                 SEARCH_BY_DIRECTOR);
                }
                else
                {
                    send_message(client->socket,
                                 "ERROR FilmBot Invalid /director command "
                                 "format. Use /director [director].\n");
                    write_to_file(client->name,
                                  "Invalid /director command format. Use "
                                  "/director [director].");
                }
            }
            else
            {

                client->count_ban++;
                if (client->count_ban == 3)
                {
                    client->ban = true;
                    ban_ip(client->ip); // Блокируем IP-адрес
                    send_message(client->socket,
                                 "BAN FilmBot You have been banned due to "
                                 "multiple violations.");
                    close(client->socket);

                    pthread_mutex_lock(&clients_mutex);
                    for (int i = 0; i < client_count; i++)
                    {
                        if (clients[i].socket == client->socket)
                        {
                            for (int j = i; j < client_count - 1; j++)
                            {
                                clients[j] = clients[j + 1];
                            }
                            client_count--;
                            break;
                        }
                    }
                    pthread_mutex_unlock(&clients_mutex);

                    break;
                }
                send_message(client->socket,
                             "ERROR FilmBot Invalid command. Print /help;");
            }
        }
    }

    return NULL;
}