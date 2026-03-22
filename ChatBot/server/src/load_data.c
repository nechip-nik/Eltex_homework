#include "../include/datafileparser.h"
/**
 * @brief Загружает данные из файла в указанный массив.
 *
 * Данная функция открывает файл с заданным именем, считывает его
 * содержимое построчно и загружает данные в массив, указанный в параметре
 *'data'. Тип данных, который будет загружен определяется параметром 'type'.
 * Функция поддерживает 3 типа данных:
 * структуры FILM, структуры JOKE_QUOTE и массив строк для info/help.
 *
 * @param filename - имя файла, из которого будут загруженны данные.
 * @param data - указатель на указатель, в который будет записан адрес
 *				 выделенной памяти для хранения загруженных данных.
 * @param count - указатель на переменную, в которую будет записано
 *				  количество загруженных элементов.
 * @param type - тип данных, которые будут загружены:
 *				 type_film - структуры FILM,
 * 				 type_joke_quote - структуры JOKE_QUOTE,
 *				 type_info_help - массив строк.
 *
 * @return 0 в случае успешного выполнения, или -1 в случае ошибки,
 *         например, если файл не удалось открыть, если не удалось выделить
 *		   память, или если в файле обнаружены некорректные строки.
 */
int load_data(const char *filename, void **data, int *count, int type)
{
    FILE *file = fopen(filename, "r");
    if (!file)
    {
        fprintf(stderr, "Не удалось открыть файл %s\n", filename);
        return -1;
    }

    char line[MAX_FILE_LINE];
    int line_count = 0;

    while (fgets(line, MAX_FILE_LINE, file))
    {
        line_count++;
    }

    rewind(file);

    if (type == type_film)
    {
        *data = malloc(line_count * sizeof(struct FILM));
    }
    else if (type == type_joke_quote)
    {
        *data = malloc(line_count * sizeof(struct JOKE_QUOTE));
    }
    else
    {
        *data = malloc(line_count * sizeof(char *));
    }

    if (!*data)
    {
        fprintf(stderr, "Неудалось выделить память под массив структур\n");
        fclose(file);
        return -1;
    }

    while (fgets(line, MAX_FILE_LINE, file))
    {
        line[strcspn(line, "\n")] = '\0';

        if (type == type_film)
        {
            struct FILM *current_film = &((struct FILM *)*data)[*count];
            // Очищаем строковые поля
            current_film->name[0] = '\0';
            current_film->genre[0] = '\0';
            current_film->year[0] = '\0';
            current_film->director[0] = '\0';

            if (parse_film(line, current_film) != 0)
            {
                fprintf(stderr, "Обнаружена некорректная строка в файле %s\n",
                        filename);
                break;
            }
        }
        else if (type == type_joke_quote)
        {
            struct JOKE_QUOTE *current_joke_quote =
                &((struct JOKE_QUOTE *)*data)[*count];
            current_joke_quote->content[0] = '\0';
            current_joke_quote->type[0] = '\0';

            if (parse_joke_quote(line, current_joke_quote) != 0)
            {
                fprintf(stderr, "Обнаружена некорректная строка в файле %s\n",
                        filename);
                break;
            }
        }
        else
        {
            ((char **)*data)[*count] = malloc(strlen(line) + 1);
            if (!((char **)*data)[*count])
            {
                fprintf(stderr,
                        "Неудалось выделить память под строку info/help\n");
                break;
            }
            strcpy(((char **)*data)[*count], line);
        }

        (*count)++;
    }

    fclose(file);

    if (*count != line_count)
    {
        fprintf(stderr, "Номер некорректной строки: %d\n", ++(*count));
        return -1;
    }

    return 0;
}