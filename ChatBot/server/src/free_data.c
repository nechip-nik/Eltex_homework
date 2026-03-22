#include "../include/datafileparser.h"
/**
 * @brief Освобождает память, выделенную для массива структур FILM.
 *
 * Данная функция проходит по массиву структур FILM и освобождает память,
 * выделенную для каждого поля (name, genre, year, director) каждой структуры.
 * После освобождения памяти для всех полей, функция также освобождает память,
 * выделенную для самого массива структур.
 *
 * @param films - указатель на массив структур FILM.
 * @param film_count - количество фильмов в массиве.
 */
void free_films(struct FILM *films, int film_count)
{
    for (int i = 0; i < film_count; i++)
    {
        if (films[i].name)
        {
            free(films[i].name);
        }
        if (films[i].genre)
        {
            free(films[i].genre);
        }
        if (films[i].year)
        {
            free(films[i].year);
        }
        if (films[i].director)
        {
            free(films[i].director);
        }
    }
    free(films);
}

/**
 * @brief Освобождает память, выделенную для массива структур JOKE_QUOTE.
 * Данная функция проходит по массиву структур JOKE_QUOTE и освобождает память,
 * выделенную для каждого поля (content, type) каждой структуры.
 * Поле освобождения памяти для всех полей, функция также освобождает память,
 * ыделенную для самого массиву структур.
 *
 * @param jokes_quotes - указатель на массив структур JOKE_QUOTE.
 * @param joke_quote_count - количество шуток/цитат в массиве.
 */
void free_jokes_quotes(struct JOKE_QUOTE *jokes_quotes, int joke_quote_count)
{
    for (int i = 0; i < joke_quote_count; i++)
    {
        if (jokes_quotes[i].content)
        {
            free(jokes_quotes[i].content);
        }
        if (jokes_quotes[i].type)
        {
            free(jokes_quotes[i].type);
        }
    }
    free(jokes_quotes);
}

/**
 * @brief Освобождает память, выделенную для массива строк.
 * Данная функция проходит по массиву строк и освобождает память,
 * выделенную для каждой строки.
 * Поле освобождения памяти для всех строк, функция также освобождает память,
 * ыделенную для самого массиву.
 *
 * @param info_help - указатель на массив строк.
 * @param info_help_count - количество строк в массиве.
 */
void free_info_help(char **info_help, int info_help_count)
{
    for (int i = 0; i < info_help_count; i++)
    {
        if (info_help[i])
        {
            free(info_help[i]);
        }
    }
    free(info_help);
}
