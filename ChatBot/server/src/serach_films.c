#include "../include/datafileparser.h"
#include "../include/lib_network.h"
#include "../include/lib_user.h"

char message[256];

void search_films(const char *username, int client_socket,
                  const char *search_term, enum SearchType type)
{
    void *data = NULL;
    int count = 0;

    char *filename = "server/data/films.txt";

    if (load_data(filename, &data, &count, type_film) != 0)
    {
        fprintf(stderr, "Ошибка при загрузке данных из файла\n");
        return;
    }

    if (data == NULL || count == 0)
    {
        fprintf(stderr, "Ошибка: загружены пустые данные\n");
        return;
    }

    struct FILM *films = (struct FILM *)data;

    printf("%d", count);
    for (int i = 0; i < count; i++)
    {
        int match = 0;

        // Проверка перед strstr() - если NULL, пропустить
        switch (type)
        {
        case SEARCH_BY_NAME:
            if (films[i].name && strstr(films[i].name, search_term) != NULL)
            {
                match = 1;
            }
            break;
        case SEARCH_BY_DIRECTOR:
            if (films[i].director &&
                strstr(films[i].director, search_term) != NULL)
            {
                match = 1;
            }
            break;
        case SEARCH_BY_YEAR:
            if (films[i].year && strstr(films[i].year, search_term) != NULL)
            {
                match = 1;
            }
            break;
        case SEARCH_BY_GENRE:
            if (films[i].genre && strstr(films[i].genre, search_term) != NULL)
            {
                match = 1;
            }
            break;
        default:
            printf("Неизвестный тип поиска\n");
            free(data);
            return;
        }

        if (match)
        {
            snprintf(
                message, sizeof(message),
                "MESSAGE FilmBot Name: %s, Genre: %s, Year: %s, Director: %s\n",
                films[i].name, films[i].genre, films[i].year,
                films[i].director);

            // Теперь переменная `message` содержит объединенную строку
            printf("%s", message); // Вывод для проверки
            send_message(client_socket, message);
            write_to_file(username, message);
            sleep(0.5);
        }
    }
}
