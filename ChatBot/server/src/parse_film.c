#include "../include/datafileparser.h"
/**
 * @brief Парсит строку и заполняет структуру FILM.
 *
 * Данная функция принимает строку, содержащую информацию о фильме, и заполняет
 * соответствующие поля структуры FILM. Строка должна быть разделена символом
 * "|" и содержать следующие поля в порядке:
 * идентификатор, название, жанр, год выпуска и режиссер.
 *
 * @param line - указатель на строку, содержащую данные о фильме.
 * @param film - указатель на структуру FILM, которую нужно заполнить.
 *
 * @return 0 в случае успешного выполнения, или -1 в случае ошибки,
 *         например, если строка некорректна или если не удалось
 *		   выделить память для строковых полей.
 */
int parse_film(char *line, struct FILM *film)
{
    char *token;
    char *endptr;
    char line_copy[MAX_FILE_LINE];

    // Создаем копию строки для strtok
    strncpy(line_copy, line, MAX_FILE_LINE);
    line_copy[MAX_FILE_LINE - 1] = '\0'; // защита от переполнения

    // id
    token = strtok(line_copy, "|");
    if (!token)
    {
        return -1;
    }
    film->id = strtol(token, &endptr, 10);
    if (*endptr != '\0' && *endptr != '\n')
    {
        fprintf(stderr, "Некорректный идентификатор фильма\n");
        return -1;
    }

    // name
    token = strtok(NULL, "|");
    while (token && *token == ' ')
        token++; // Убираем ведущие пробелы
    if (!token)
    {
        fprintf(stderr, "Некорректное название фильма\n");
        return -1;
    }
    strncpy(film->name, token, sizeof(film->name) - 1);
    film->name[sizeof(film->name) - 1] = '\0'; // защита от переполнения

    // genre
    token = strtok(NULL, "|");
    while (token && *token == ' ')
        token++; // Убираем ведущие пробелы
    if (!token)
    {
        fprintf(stderr, "Некорректный жанр фильма\n");
        return -1;
    }
    strncpy(film->genre, token, sizeof(film->genre) - 1);
    film->genre[sizeof(film->genre) - 1] = '\0'; // защита от переполнения

    // year
    token = strtok(NULL, "|");
    while (token && *token == ' ')
        token++; // Убираем ведущие пробелы
    if (!token)
    {
        fprintf(stderr, "Некорректный год выпуска фильма в прокат\n");
        return -1;
    }
    strncpy(film->year, token, sizeof(film->year) - 1);
    film->year[sizeof(film->year) - 1] = '\0'; // защита от переполнения

    // director
    token = strtok(NULL, "|");
    while (token && *token == ' ')
        token++; // Убираем ведущие пробелы
    if (!token)
    {
        fprintf(stderr, "Некорректный режиссер фильма\n");
        return -1;
    }
    strncpy(film->director, token, sizeof(film->director) - 1);
    film->director[sizeof(film->director) - 1] = '\0'; // защита от переполнения

    return 0;
}
