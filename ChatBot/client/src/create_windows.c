#include "../include/messanger.h"

void create_windows()
{
    message_win_height = LINES - 6; // Высота окна сообщений
    message_win_width = COLS - 2;   // Ширина окна сообщений
    input_win_height = 5;           // Высота окна ввода
    input_win_width = COLS;         // Ширина окна ввода

    // Создание окон с рамками
    message_win_border =
        newwin(message_win_height + 1, message_win_width + 2, 0, 0);
    input_win_border =
        newwin(input_win_height, input_win_width, LINES - input_win_height, 0);

    // Создание внутренних окон
    message_win = newwin(message_win_height - 1, message_win_width, 1, 1);
    input_win = newwin(input_win_height - 2, input_win_width - 2,
                       LINES - input_win_height + 1, 1);

    // Включение скролла для окон
    scrollok(message_win, TRUE);
    scrollok(input_win, TRUE);

    // Рисование рамок
    box(message_win_border, 0, 0);
    box(input_win_border, 0, 0);

    // Обновление окон
    wrefresh(message_win_border);
    wrefresh(input_win_border);
}