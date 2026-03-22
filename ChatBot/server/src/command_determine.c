/*
 *	Команды которые может определить функция:
 *		1 /help
 *		2 /info
 *		3 /exit
 *		4 /quote "1/0" (1 - научная цитата; 0 - литературная цитата)
 *		5 /new_quote "1/0" "текст цитаты"
 *		6 /joke "1/0" (1 - смешная шутка; 0 - несмешная шутка)
 *		7 /new_joke "1/0" "текст шутки"
 *		8 /name "название фильма"
 *		9 /genre "жанр фильма"
 *		10 /year "год фильма"
 *		11 /director "режиссёр фильма"
 *
 *	Возвращаемые значения функции
 *		 0 успешное выполнение
 *		-1 ошибка связанная с количеством кавычек
 *		-2 неизвестная ошибка
 *		-3 ошибка определения положения ковычек (1 параметр)
 *		-4 ошибка определения положения ковычек (2 параметра)
 *		-5 ошибка выделения памяти
 *		-6 ошибка заполнения типа цитаты
 *		-7 ошибка заполнения типа шутки
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define STRING_SIZE 1000
#define JOKE_SIZE 50

struct FILM
{
    int id;
    char *name;
    char *genre;
    char *year;
    char *director;
};

struct JOKE_QUOTE
{
    int user_id;
    char *joke_or_quote;
    char *type;
};

enum Command_type
{
    UNKNOWN = 0,
    HELP = 1,
    INFO = 2,
    EXIT = 3,
    QUOTE = 4,
    NEW_QUOTE = 5,
    JOKE = 6,
    NEW_JOKE = 7,
    NAME = 8,
    GENRE = 9,
    YEAR = 10,
    DIRECTOR = 11
};

int command_determine(struct FILM *films, int films_count,
                      struct JOKE_QUOTE **jokes, int *joke_count,
                      struct JOKE_QUOTE **quotes, int *quote_count,
                      int user_index, char *help, char *info,
                      const char *command, char *response)
{
    int quotes_count = 0;
    int quote_index[4] = {-1, -1, -1, -1};
    srand(time(0));
    int random_content;
    enum Command_type command_type = UNKNOWN;
    char command_parametr_1[100] = "";
    char command_parametr_2[100] = "";
    char *command_ptr = command;
    int command_parametr_lenght = 0;

    if (strncmp(command, "/help", 5) == 0)
    {
        command_type = HELP;
        printf("help - yes\n");
        strcpy(response, help);
        return 1;
    }

    if (strncmp(command, "/info", 5) == 0)
    {
        command_type = INFO;
        printf("info - yes\n");
        strcpy(response, info);
        return 1;
    }

    if (strncmp(command, "/exit", 5) == 0)
    {
        command_type = EXIT;
        strcpy(response, "Выход - есть выход");
        printf("exit - yes\n");
        return 1;
    }

    /*определения кол-ва двойных ковычек*/
    while (*command_ptr != '\0')
    {
        if (*command_ptr == '"')
        {
            quotes_count++;
        }
        command_ptr++;
    }

    /*2 двойные ковычки для одного параметра и 4 для двух парам-ов*/
    if ((quotes_count == 2) || (quotes_count == 4))
    {
        int quote_index_i = 0;
        for (int j = 0; command[j] != '\0'; j++)
        {
            if (command[j] == '\"')
            {
                quote_index[quote_index_i] = j;
                quote_index_i++;
            }
        }
    }
    else
    {
        /*-1 ошибка связанная с количеством кавычек*/
        return -1;
    }

    if (quote_index[0] != -1 && quote_index[1] != -1)
    {
        command_parametr_lenght = quote_index[1] - quote_index[0];
        strncpy(command_parametr_1, command + quote_index[0] + 1,
                command_parametr_lenght - 1);
        command_parametr_1[command_parametr_lenght] = '\0';
        printf("command_parametr_lenght: %d\n", command_parametr_lenght);
    }
    else
    {
        return -3;
    }

    if ((strncmp(command, "/quote", 6) == 0) && (quotes_count == 2))
    {
        /*вызов функции для команды /name*/
        printf("Команда: /quote\n");
        printf("Параметр: %s\n", command_parametr_1);
        command_type = QUOTE;

        int *select_array = malloc(*quote_count * sizeof(int));

        /*проверка успешного выделения памяти*/
        if (select_array == NULL)
        {
            printf("Ошибка выделения памяти!\n");
            return -2;
        }
        int select_count = 0;
        for (int i = 0; i < *quote_count; i++)
        {
            if (((strcmp(quotes[i]->type, command_parametr_1)) == 0) &&
                ((quotes[i]->user_id == user_index) ||
                 (quotes[i]->user_id == 0)))
            {
                select_array[select_count] = i;
                select_count++;
                printf("Индекс с нужным =%d\n", i);
            }
        }

        for (int i = 0; i < select_count; i++)
        {
            fprintf(stdout, "select_array[%d]=%d\n", i, select_array[i]);
        }

        for (int i = 0; i < select_count; i++)
        {
            strcat(response, quotes[select_array[i]]->joke_or_quote);
            strcat(response, "|");
            strcat(response, quotes[select_array[i]]->type);
            strcat(response, "|");
        }
        free(select_array);
        return 0;
    }

    if (strncmp(command, "/joke", 5) == 0 && (quotes_count == 2))
    {
        printf("Команда: /joke\n");
        printf("Параметр: %s\n", command_parametr_1);
        command_type = JOKE;

        int *select_array = malloc(*joke_count * sizeof(int));

        /*проверка успешного выделения памяти*/
        if (select_array == NULL)
        {
            printf("Ошибка выделения памяти!\n");
            return -2;
        }

        int select_count = 0;

        for (int i = 0; i < *joke_count; i++)
        {
            if (((strcmp(jokes[i]->type, command_parametr_1)) == 0) &&
                ((jokes[i]->user_id == user_index) || (jokes[i]->user_id == 0)))
            {
                select_array[select_count] = i;
                select_count++;
                printf("Индекс с нужным =%d\n", i);
            }
        }

        for (int i = 0; i < select_count; i++)
        {
            fprintf(stdout, "select_array[%d]=%d\n", i, select_array[i]);
        }

        for (int i = 0; i < select_count; i++)
        {
            strcat(response, jokes[select_array[i]]->joke_or_quote);
            strcat(response, "|");
            strcat(response, jokes[select_array[i]]->type);
            strcat(response, "|");
        }

        free(select_array);
        return 0;
    }

    if (strncmp(command, "/name", 5) == 0)
    {
        /*вызов функции для команды /name*/
        printf("Команда: /name\n");
        printf("Параметр: %s\n", command_parametr_1);
        command_type = NAME;
        for (int i = 0; i < films_count; i++)
        {
            if ((strcmp(films[i].name, command_parametr_1)) == 0)
            {
                strcat(response, films[i].name);
                strcat(response, "|");
                strcat(response, films[i].genre);
                strcat(response, "|");
                strcat(response, films[i].year);
                strcat(response, "|");
                strcat(response, films[i].director);
                strcat(response, "|");
            }
        }
        return 0;
    }

    if (strncmp(command, "/genre", 6) == 0)
    {
        /*вызов функции для команды /genre*/
        printf("Команда: /genre\n");
        printf("Параметр: %s\n", command_parametr_1);
        command_type = GENRE;
        for (int i = 0; i < films_count; i++)
        {
            if ((strcmp(films[i].genre, command_parametr_1)) == 0)
            {
                strcat(response, films[i].name);
                strcat(response, "|");
                strcat(response, films[i].genre);
                strcat(response, "|");
                strcat(response, films[i].year);
                strcat(response, "|");
                strcat(response, films[i].director);
                strcat(response, "|");
            }
        }
        return 0;
    }

    if (strncmp(command, "/year", 5) == 0)
    {
        /*вызов функции для команды /year*/
        printf("Команда: /year\n");
        printf("Параметр: %s\n", command_parametr_1);
        command_type = YEAR;
        for (int i = 0; i < films_count; i++)
        {
            if ((strcmp(films[i].year, command_parametr_1)) == 0)
            {
                strcat(response, films[i].name);
                strcat(response, "|");
                strcat(response, films[i].genre);
                strcat(response, "|");
                strcat(response, films[i].year);
                strcat(response, "|");
                strcat(response, films[i].director);
                strcat(response, "|");
            }
        }
        return 0;
    }

    if (strncmp(command, "/director", 9) == 0)
    {
        /*вызов функции для команды /director*/
        printf("Команда: //director\n");
        printf("Параметр: %s\n", command_parametr_1);
        command_type = DIRECTOR;
        for (int i = 0; i < films_count; i++)
        {
            if ((strcmp(films[i].director, command_parametr_1)) == 0)
            {
                strcat(response, films[i].name);
                strcat(response, "|");
                strcat(response, films[i].genre);
                strcat(response, "|");
                strcat(response, films[i].year);
                strcat(response, "|");
                strcat(response, films[i].director);
                strcat(response, "|");
            }
        }
        return 0;
    }

    if (quotes_count == 4)
    {
        if ((quote_index[0] != -1) && (quote_index[1] != -1) &&
            (quote_index[2] != -1) && (quote_index[3] != -1))
        {
            command_parametr_lenght = quote_index[1] - quote_index[0];
            strncpy(command_parametr_1, command + quote_index[0] + 1,
                    command_parametr_lenght - 1);
            command_parametr_1[command_parametr_lenght] = '\0';

            command_parametr_lenght = quote_index[3] - quote_index[2];
            strncpy(command_parametr_2, command + quote_index[2] + 1,
                    command_parametr_lenght - 1);
            command_parametr_2[command_parametr_lenght] = '\0';
        }
        else
        {
            return -4;
        }

        if (strncmp(command, "/new_quote", 10) == 0 && (quotes_count == 4))
        {
            printf("Команда: /new_quote\n");
            printf("Параметр 1: %s\n", command_parametr_1);
            printf("Параметр 2: %s\n", command_parametr_2);
            command_type = NEW_QUOTE;

            if ((strncmp(command_parametr_1, "научная", 7) != 0) &&
                (strncmp(command_parametr_1, "литературная", 12) != 0))
            {
                printf("Ошибка заполнения типа цитаты\n");
                return -6;
            }

            *quotes = realloc(*quotes,
                              (*quote_count + 1) * sizeof(struct JOKE_QUOTE));

            if (*quotes == NULL)
            {
                printf("Ошибка выделения памяти!\n");
                return -5;
            }

            (*quotes)[*quote_count].user_id = user_index;
            (*quotes)[*quote_count].joke_or_quote =
                malloc(JOKE_SIZE * sizeof(char));
            (*quotes)[*quote_count].type = malloc(JOKE_SIZE * sizeof(char));

            strcpy((*quotes)[*quote_count].joke_or_quote, command_parametr_2);
            strcpy((*quotes)[*quote_count].type, command_parametr_1);

            // увеличиваем количество шуток
            (*quote_count)++;

            return 0;
        }

        if (strncmp(command, "/new_joke", 9) == 0 && (quotes_count == 4))
        {
            printf("Команда: /new_joke\n");
            printf("Параметр 1: %s\n", command_parametr_1);
            printf("Параметр 2: %s\n", command_parametr_1);
            command_type = NEW_JOKE;

            if ((strncmp(command_parametr_1, "смешная", 7) != 0) &&
                (strncmp(command_parametr_1, "несмешная", 9) != 0))
            {
                printf("Ошибка заполнения типа шутки\n");
                return -7;
            }

            *jokes =
                realloc(*jokes, (*joke_count + 1) * sizeof(struct JOKE_QUOTE));

            if (*jokes == NULL)
            {
                printf("Ошибка выделения памяти!\n");
                return -5;
            }

            (*jokes)[*joke_count].user_id = user_index;
            (*jokes)[*joke_count].joke_or_quote =
                malloc(JOKE_SIZE * sizeof(char));
            (*jokes)[*joke_count].type = malloc(JOKE_SIZE * sizeof(char));

            strcpy((*jokes)[*joke_count].joke_or_quote, command_parametr_2);
            strcpy((*jokes)[*joke_count].type, command_parametr_1);

            // увеличиваем количество шуток
            (*joke_count)++;

            return 0;
        }
    }
    /*-2 неизвестная ошибка*/
    return -2;
}