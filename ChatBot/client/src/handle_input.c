#include "../include/messanger.h"

void handle_input()
{
    int ch;
    int input_index = strlen(input_message);

    while ((ch = wgetch(input_win)) != '\n')
    {
        catch (ch);

        if (ch == KEY_BACKSPACE || ch == 127 || ch == '\b')
        {
            if (input_index > 0)
            {
                input_message[--input_index] = '\0';
            }
        }
        else if (ch == KEY_RESIZE)
        {
            create_windows();
            display_messages();
            display_input();
        }
        else if (is_key_up())
        {
            input_index -= 2;
            input_message[input_index] = '\0';

            int total_lines =
                calculate_total_lines(); // Получаем общее количество строк
            if (message_scroll_pos < total_lines - message_win_height)
            {
                message_scroll_pos++;
                display_messages();
            }
        }
        else if (is_key_down())
        {
            input_index -= 2;
            input_message[input_index] = '\0';

            if (message_scroll_pos > 0)
            {
                message_scroll_pos--;
                display_messages();
            }
        }
        else if (input_index < MAX_MESSAGE_LEN - 1)
        {
            input_message[input_index++] = ch;
            input_message[input_index] = '\0';
        }
        display_input();
    }

    if (input_index > 0)
    {
        // Отправляем сообщение на сервер
        if (strncmp(input_message, "/exit", 5) == 0)
        {
            send_message_to_server("/exit");
            close(sockfd);
            endwin();
            exit(0); // Закрытие клиента
        }
        else
        {
            send_message_to_server(input_message);
            add_message(current_user, input_message);
            display_messages();
        }
        input_message[0] = '\0';
        display_input();
    }
}