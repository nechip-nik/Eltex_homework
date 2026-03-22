#include "../include/datafileparser.h"

int parse_joke_quote(char *line, struct JOKE_QUOTE *joke_quote)
{
    char *token;

    // Инициализация поля id нулями (или любым другим значением по умолчанию)
    joke_quote->user_id = 0; // Заполняем id нулями

    // type
    token = strtok(line, "|");
    if (!token)
    {
        fprintf(stderr, "Некорректный тип шутки/цитаты\n");
        return -1;
    }
    // Копируем строку в массив type
    strncpy(joke_quote->type, token, sizeof(joke_quote->type) - 1);
    joke_quote->type[sizeof(joke_quote->type) - 1] =
        '\0'; // защита от переполнения

    // content
    token = strtok(NULL, "|");
    if (!token)
    {
        fprintf(stderr, "Некорректное содержание шутки/цитаты\n");
        return -1;
    }
    // Копируем строку в массив content
    strncpy(joke_quote->content, token, sizeof(joke_quote->content) - 1);
    joke_quote->content[sizeof(joke_quote->content) - 1] =
        '\0'; // защита от переполнения

    return 0;
}