#ifndef DATAFILEPARSER_H
#define DATAFILEPARSER_H

#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILE_LINE 1024

#define type_film 0
#define type_joke_quote 1
#define type_info_help 2

struct FILM
{
    int id;
    char name[256];
    char genre[256];
    char year[256];
    char director[256];
};

struct JOKE_QUOTE
{
    int user_id;
    char content[256];
    char type[32];
};

enum SearchType
{
    SEARCH_BY_NAME,
    SEARCH_BY_DIRECTOR,
    SEARCH_BY_YEAR,
    SEARCH_BY_GENRE
};

int load_data(const char *filename, void **data, int *count, int type);
int parse_film(char *line, struct FILM *film);
int parse_joke_quote(char *line, struct JOKE_QUOTE *joke_quote);
void free_films(struct FILM *films, int film_count);
void free_jokes_quotes(struct JOKE_QUOTE *jokes_quotes, int joke_quote_count);
void free_info_help(char **info_help, int info_help_count);
void search_films(const char *username, int client_socket,
                  const char *search_term, enum SearchType type);
void search_quotes(const char *username, int client_socket,
                   const char *search_type, int user_id,
                   struct JOKE_QUOTE *user_quotes, int user_joke_count);
void search_jokes(const char *username, int client_socket,
                  const char *search_type, int user_id,
                  struct JOKE_QUOTE *user_jokes, int user_joke_count);

#endif
