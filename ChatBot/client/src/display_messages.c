#include "messanger.h"

// Функция для подсчёта общего количества строк в сообщениях
int calculate_total_lines()
{
    int total_lines = 0;
    for (int i = 0; i < chatroom.message_count; i++)
    {
        const char *msg = chatroom.messages[i].message;
        int msg_length = strlen(msg);
        const char *username = chatroom.messages[i].username;
        int username_length = strlen(username);

        // Каждая строка с именем пользователя
        total_lines += 1;

        // Обработка сообщения: переносы по ';' и ширине окна
        int current_pos = 0;
        int line_count = 0;
        while (current_pos < msg_length)
        {
            // Ищем ближайший ';' или конец строки
            int next_semicolon = -1;
            for (int j = current_pos; j < msg_length; j++)
            {
                if (msg[j] == ';')
                {
                    next_semicolon = j;
                    break;
                }
            }

            // Определяем длину текущего сегмента
            int segment_length;
            if (next_semicolon != -1)
            {
                segment_length = next_semicolon - current_pos;
            }
            else
            {
                segment_length = msg_length - current_pos;
            }

            // Разбиваем сегмент на части по ширине окна
            int lines_in_segment =
                (segment_length + message_win_width - 1) / message_win_width;
            line_count += lines_in_segment;

            current_pos += segment_length;
            if (next_semicolon != -1)
            {
                current_pos++; // Пропускаем ';'
                line_count++;  // Перенос строки из-за ';'
            }
        }

        total_lines += line_count + 1; // +1 пустая строка между сообщениями
    }
    return total_lines;
}

void display_messages()
{
    werase(message_win);

    int total_lines = calculate_total_lines();
    int start_line = (total_lines > message_win_height)
                         ? total_lines - message_win_height - message_scroll_pos
                         : 0;
    if (start_line < 0)
        start_line = 0;

    int current_line = 0;
    for (int i = 0; i < chatroom.message_count; i++)
    {
        // Вывод имени пользователя
        if (current_line >= start_line &&
            current_line < start_line + message_win_height)
        {
            wattron(message_win, COLOR_PAIR(1));
            waddstr(message_win, chatroom.messages[i].username);
            waddstr(message_win, ": ");
            wattroff(message_win, COLOR_PAIR(1));
        }
        current_line++;

        const char *msg = chatroom.messages[i].message;
        int msg_length = strlen(msg);
        int msg_pos = 0;

        wattron(message_win, COLOR_PAIR(2));
        while (msg_pos < msg_length)
        {
            // Поиск следующего ';'
            int next_semicolon = -1;
            for (int j = msg_pos; j < msg_length; j++)
            {
                if (msg[j] == ';')
                {
                    next_semicolon = j;
                    break;
                }
            }

            // Обработка текущего сегмента (до ';' или конца)
            int segment_length;
            if (next_semicolon != -1)
            {
                segment_length = next_semicolon - msg_pos;
            }
            else
            {
                segment_length = msg_length - msg_pos;
            }

            // Разбиваем сегмент на строки по ширине окна
            int segment_pos = 0;
            while (segment_pos < segment_length)
            {
                int remaining = segment_length - segment_pos;
                int line_len = (remaining > message_win_width)
                                   ? message_win_width
                                   : remaining;

                if (current_line >= start_line &&
                    current_line < start_line + message_win_height)
                {
                    waddnstr(message_win, msg + msg_pos + segment_pos,
                             line_len);
                }

                segment_pos += line_len;
                current_line++;
            }

            // Перенос строки из-за ';'
            if (next_semicolon != -1)
            {
                if (current_line >= start_line &&
                    current_line < start_line + message_win_height)
                {
                    waddch(message_win, '\n');
                }
                current_line++;
                msg_pos = next_semicolon + 1; // Пропускаем ';'
            }
            else
            {
                msg_pos = msg_length;
            }
        }

        // Пустая строка между сообщениями
        if (current_line >= start_line &&
            current_line < start_line + message_win_height)
        {
            waddch(message_win, '\n');
        }
        current_line++;
        wattroff(message_win, COLOR_PAIR(2));
    }

    wrefresh(message_win);
}