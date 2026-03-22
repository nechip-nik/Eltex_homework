#ifndef LIB_USER_H
#define LIB_USER_H

#include <arpa/inet.h> // Для INET_ADDRSTRLEN
#include <stdbool.h>

#define MAX_COUNT_USERS 10
#define MAX_NAME_LENGTH 32
#define MAX_PASSWORD_LENGTH 32
#define MAX_BANNED_IPS 100

typedef enum
{
    USER_SUCCESS = 0,       // Успешное выполнение
    USER_ERROR_EMPTY_LOGIN, // Логин пуст
    USER_ERROR_SHORT_LOGIN, // Логин слишком короткий
    USER_ERROR_INVALID_LOGIN, // Логин содержит спецсимволы
    USER_ERROR_EMPTY_PASSWORD, // Пароль пуст
    USER_ERROR_SHORT_PASSWORD, // Пароль слишком короткий
    USER_ERROR_MAX_USERS, // Достигнуто максимальное количество пользователей
    USER_ERROR_EXISTS, // Пользователь с таким именем уже существует
    USER_ERROR_BANNED, // Пользователь забанен
    USER_ERROR_NOT_FOUND, // Пользователь не найден
    USER_ERROR_INVALID_INPUT // Некорректные входные данные
} UserErrorCode;

typedef enum
{
    ACTION_ANONYMOUS, // Пользователь не зарегистрирован
    ACTION_REGISTERED, // Пользователь зарегистрирован
    ACTION_LOGGED_IN   // Пользователь вошел в систему
} UserAction;

typedef struct
{
    int id;
    int socket;
    int count_ban;
    char name[MAX_NAME_LENGTH];
    char password[MAX_PASSWORD_LENGTH];
    char ip[INET_ADDRSTRLEN]; // Поле для IP-адреса
    bool ban;
    UserAction active;
} User;

// Функции
UserErrorCode registration_user(const char *name, const char *password);
UserErrorCode login_user(const char *name, const char *password);
void ban_ip(const char *ip);
int is_ip_banned(const char *ip);
void send_help_info_file(const char *filename, int socket,
                         const char *file_path);
void send_formatted_message(int socket, const char *text);
int write_to_file(const char *filename, const char *str);
void delete_user_history_files();
#endif