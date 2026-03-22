#include "../include/datafileparser.h"
#include "../include/lib_network.h"
#include "../include/lib_user.h"

void search_jokes(const char *username, int client_socket,
                  const char *search_type, int user_id,
                  struct JOKE_QUOTE *user_jokes, int user_joke_count)
{
    void *file_data = NULL;
    int file_count = 0;

    // Загружаем шутки из файла
    char *filename = "server/data/jokes.txt"; // Путь к файлу с шутками
    if (load_data(filename, &file_data, &file_count, type_joke_quote) != 0)
    {
        fprintf(stderr, "Ошибка при загрузке данных из файла\n");
        return;
    }

    printf("Загрузка из файла завершена успешно\n");

    // Преобразуем загруженные данные в массив структур JOKE_QUOTE
    struct JOKE_QUOTE *file_jokes = (struct JOKE_QUOTE *)file_data;

    // Общее количество шуток (пользовательские + из файла)
    int total_joke_count = user_joke_count + file_count;

    // Создаем временный массив для объединенных шуток
    struct JOKE_QUOTE *all_jokes =
        malloc((total_joke_count + 5) * sizeof(struct JOKE_QUOTE));
    if (!all_jokes)
    {
        fprintf(stderr, "Ошибка: не удалось выделить память\n");
        free(file_data);
        return;
    }

    // Копируем пользовательские шутки в общий массив
    for (int i = 0; i < user_joke_count; i++)
    {
        all_jokes[i] = user_jokes[i];
    }

    // Копируем шутки из файла в общий массив
    for (int i = 0; i < file_count; i++)
    {
        all_jokes[user_joke_count + i] = file_jokes[i];
    }

    // Выводим весь массив шуток после склеивания
    printf("Все шутки после склеивания массивов:\n");
    for (int i = 0; i < total_joke_count; i++)
    {
        printf("Шутка %d:\n", i + 1);
        printf("  User ID: %d\n", all_jokes[i].user_id);
        printf("  Type: %s\n", all_jokes[i].type);
        printf("  Content: %s\n", all_jokes[i].content);
        printf("----------------------------\n");
    }

    // Создаем массив для хранения индексов подходящих шуток
    int *matching_indices = malloc(100 * sizeof(int));
    if (!matching_indices)
    {
        fprintf(stderr, "Ошибка: не удалось выделить память\n");
        free(all_jokes);
        free(file_data);
        return;
    }

    int matching_count = 0; // Количество подходящих шуток

    // Собираем индексы всех подходящих шуток
    for (int i = 0; i < total_joke_count; i++)
    {
        // Проверяем тип шутки и доступность для пользователя
        if (strcmp(all_jokes[i].type, search_type) == 0 &&
            (all_jokes[i].user_id == user_id || all_jokes[i].user_id == 0))
        { // user_id == 0 для общедоступных шуток
            matching_indices[matching_count] = i;
            matching_count++;
        }
    }

    if (matching_count == 0)
    {
        fprintf(stderr, "Ошибка: нет подходящих шуток\n");
        free(matching_indices);
        free(all_jokes);
        free(file_data);
        return;
    }

    // Инициализация генератора случайных чисел
    srand(time(NULL));

    // Выбираем случайный индекс из массива matching_indices
    int random_index = matching_indices[rand() % matching_count];

    // Формируем сообщение для отправки клиенту
    char message[512];
    snprintf(message, sizeof(message),
             "MESSAGE JokeBot  Type: %s, Content: %s\n",
             all_jokes[random_index].type, all_jokes[random_index].content);

    printf("%s", message); // Вывод для проверки
    send_message(client_socket, message);
    write_to_file(username, message);

    // Освобождаем память
    free(matching_indices);
    free(all_jokes);
    free(file_data);
}