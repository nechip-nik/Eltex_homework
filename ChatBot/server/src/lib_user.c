#include "../include/lib_user.h"
#include <dirent.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

User users[MAX_COUNT_USERS];
extern int user_count = 0;
char banned_ips[MAX_BANNED_IPS][INET_ADDRSTRLEN];
int banned_ip_count = 0;
pthread_mutex_t banned_ips_mutex = PTHREAD_MUTEX_INITIALIZER;

#define MAX_MESSAGE_SIZE 1024
#define MAX_FILENAME_SIZE 256

// Функция проверки содержания в строке спецсимвола
bool contains_special_chars(const char *str)
{
    const char *special_chars = "@#$%^&*";
    for (int i = 0; str[i] != '\0'; i++)
    {
        if (strchr(special_chars, str[i]) != NULL)
        {
            return true;
        }
    }
    return false;
}

// Функция проверки логина
UserErrorCode validate_login(const char *name)
{
    if (name == NULL || strlen(name) == 0)
    {
        return USER_ERROR_EMPTY_LOGIN;
    }
    if (strlen(name) <= 5)
    {
        return USER_ERROR_SHORT_LOGIN;
    }
    if (contains_special_chars(name))
    {
        return USER_ERROR_INVALID_LOGIN;
    }
    return USER_SUCCESS;
}

// Функция проверки пароля
UserErrorCode validate_password(const char *password)
{
    if (password == NULL || strlen(password) == 0)
    {
        return USER_ERROR_EMPTY_PASSWORD;
    }
    if (strlen(password) <= 5)
    {
        return USER_ERROR_SHORT_PASSWORD;
    }
    return USER_SUCCESS;
}

// Функция проверки на существования пользователя с таким именем
bool is_user_exists(const char *name)
{
    for (int i = 0; i < user_count; i++)
    {
        if (strcmp(users[i].name, name) == 0)
        {
            return true;
        }
    }
    return false;
}

// Функция регистрации нового пользователя
UserErrorCode registration_user(const char *name, const char *password)
{
    UserErrorCode login_status = validate_login(name);
    if (login_status != USER_SUCCESS)
    {
        return login_status;
    }

    UserErrorCode password_status = validate_password(password);
    if (password_status != USER_SUCCESS)
    {
        return password_status;
    }

    if (user_count >= MAX_COUNT_USERS)
    {
        return USER_ERROR_MAX_USERS;
    }

    if (is_user_exists(name))
    {
        return USER_ERROR_EXISTS;
    }

    // Регистрация нового пользователя
    users[user_count].id = user_count;
    strncpy(users[user_count].name, name, MAX_NAME_LENGTH - 1);
    users[user_count].name[MAX_NAME_LENGTH - 1] = '\0'; // Завершение строки
    strncpy(users[user_count].password, password, MAX_PASSWORD_LENGTH - 1);
    users[user_count].password[MAX_PASSWORD_LENGTH - 1] =
        '\0'; // Завершение строки
    users[user_count].ban = false;
    users[user_count].active = true;
    user_count++;

    return USER_SUCCESS;
}

// Функция авторизации пользователя
UserErrorCode login_user(const char *name, const char *password)
{
    if (name == NULL || password == NULL)
    {
        return USER_ERROR_INVALID_INPUT;
    }

    for (int i = 0; i < user_count; i++)
    {
        if (strcmp(users[i].name, name) == 0 &&
            strcmp(users[i].password, password) == 0)
        {
            if (users[i].ban)
            {
                return USER_ERROR_BANNED;
            }
            users[i].active = true;
            return USER_SUCCESS;
        }
    }

    return USER_ERROR_NOT_FOUND;
}
void ban_ip(const char *ip)
{
    pthread_mutex_lock(&banned_ips_mutex);
    if (banned_ip_count < MAX_BANNED_IPS)
    {
        strncpy(banned_ips[banned_ip_count], ip, INET_ADDRSTRLEN - 1);
        banned_ips[banned_ip_count][INET_ADDRSTRLEN - 1] = '\0';
        banned_ip_count++;
    }
    pthread_mutex_unlock(&banned_ips_mutex);
}

// Проверка, забанен ли IP-адрес
int is_ip_banned(const char *ip)
{
    pthread_mutex_lock(&banned_ips_mutex);
    for (int i = 0; i < banned_ip_count; i++)
    {
        if (strcmp(banned_ips[i], ip) == 0)
        {
            pthread_mutex_unlock(&banned_ips_mutex);
            return 1; // IP забанен
        }
    }
    pthread_mutex_unlock(&banned_ips_mutex);
    return 0; // IP не забанен
}
// Function to send a formatted message to the client
void send_formatted_message(int socket, const char *text)
{
    char formatted_message[MAX_MESSAGE_SIZE];
    snprintf(formatted_message, sizeof(formatted_message), "MESSAGE filmbot %s",
             text);
    send_message(socket, formatted_message);
}

void send_help_info_file(const char *filename, int socket,
                         const char *file_path)
{
    char message[MAX_MESSAGE_SIZE];
    char full_message[MAX_MESSAGE_SIZE * 10]; // Увеличиваем размер для хранения
                                              // всего содержимого файла
    full_message[0] = '\0'; // Инициализируем пустую строку

    FILE *file = fopen(file_path, "r");
    if (file != NULL)
    {
        while (fgets(message, sizeof(message), file) != NULL)
        {
            // Убираем символ новой строки
            size_t len = strlen(message);
            if (len > 0 && message[len - 1] == '\n')
            {
                message[len - 1] = '\0';
            }

            // Проверяем, не превысит ли добавление нового сообщения
            // максимальный размер
            if (strlen(full_message) + strlen(message) + 1 <
                sizeof(full_message))
            {
                strcat(full_message, message);
            }
            else
            {
                // Обработка ошибки переполнения буфера
                fprintf(stderr, "Error: Buffer overflow detected.\n");
                break;
            }
        }
        fclose(file);

        // Отправляем полное сообщение
        char formatted_message[MAX_MESSAGE_SIZE];
        snprintf(formatted_message, sizeof(formatted_message),
                 "MESSAGE filmbot %s", full_message);
        send_message(socket, formatted_message);

        // Записываем в файл
        write_to_file(filename, full_message);
    }
    else
    {
        // Обработка ошибки открытия файла
        char error_message[] = "Error: Could not open help file.\n";
        printf("%s", error_message);
        send_formatted_message(socket, error_message);
    }
}

int write_to_file(const char *name, const char *str)
{
    char filename[MAX_FILENAME_SIZE];
    snprintf(filename, sizeof(filename), "history/%s.txt", name);

    FILE *file = fopen(filename, "a");
    if (file == NULL)
    {
        perror("Error opening file");
        return -1;
    }

    char formatted_message[MAX_MESSAGE_SIZE];
    snprintf(formatted_message, sizeof(formatted_message), "MESSAGE filmbot %s",
             str);
    fprintf(file, "%s\n", formatted_message);

    fclose(file);
    return 0;
}
void delete_user_history_files()
{
    DIR *dir;
    struct dirent *entry;

    // Открываем директорию, где хранятся файлы истории
    dir = opendir("history");
    if (dir == NULL)
    {
        perror("Ошибка при открытии директории history");
        return;
    }

    // Читаем содержимое директории
    while ((entry = readdir(dir)) != NULL)
    {
        // Проверяем, что файл имеет расширение .txt
        if (strstr(entry->d_name, ".txt") != NULL)
        {
            char filepath[256];
            snprintf(filepath, sizeof(filepath), "history/%s", entry->d_name);

            // Удаляем файл
            if (remove(filepath) == 0)
            {
                printf("Файл '%s' удален.\n", filepath);
            }
            else
            {
                perror("Ошибка при удалении файла");
            }
        }
    }

    closedir(dir);
}