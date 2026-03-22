#include "../include/messanger.h"
void login_window()
{
    WINDOW *login_win =
        newwin(9, 50, LINES / 3,
               COLS / 3); // Увеличиваем высоту окна для дополнительного текста
    box(login_win, 0, 0);

    // Добавляем текст, который сообщает пользователю, что это окно логина
    mvwprintw(login_win, 1, 1, "Logging in...");

    mvwprintw(login_win, 2, 1, "Enter your username: ");
    wrefresh(login_win);
    echo(); // Включаем отображение текста для логина
    mvwscanw(login_win, 2, 22, "%s", login); // Вводим имя пользователя

    mvwprintw(login_win, 3, 1, "Enter your password: ");
    wrefresh(login_win);
    noecho(); // Отключаем отображение текста для пароля
    mvwgetnstr(login_win, 3, 22, password, MAX_NAME_LEN); // Вводим пароль

    noecho(); // Вновь выключаем скрытие текста
    wclear(login_win);
    delwin(login_win);
    // Логика для проверки логина может быть добавлена здесь
    strcpy(current_user, login);
}
