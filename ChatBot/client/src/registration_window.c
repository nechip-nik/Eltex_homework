#include "../include/messanger.h"
void registration_window()
{
    WINDOW *reg_win =
        newwin(9, 50, LINES / 3,
               COLS / 3); // Увеличиваем высоту окна для дополнительного текста
    box(reg_win, 0, 0);

    // Добавляем текст, который сообщает пользователю, что это окно регистрации
    mvwprintw(reg_win, 1, 1, "Registering new user...");

    mvwprintw(reg_win, 2, 1, "Enter your username: ");
    wrefresh(reg_win);
    echo(); // Включаем отображение текста для логина
    mvwscanw(reg_win, 2, 22, "%s", login); // Вводим имя пользователя

    mvwprintw(reg_win, 3, 1, "Enter your password: ");
    wrefresh(reg_win);
    noecho(); // Отключаем отображение текста для пароля
    mvwgetnstr(reg_win, 3, 22, password, MAX_NAME_LEN); // Вводим пароль
    noecho(); // Вновь выключаем скрытие текста
    wclear(reg_win);
    delwin(reg_win);
}
