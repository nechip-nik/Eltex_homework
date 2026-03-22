#include "../include/messanger.h"
int login_or_register_window()
{
    while (1)
    {
        // Создаем главное окно
        WINDOW *main_win = newwin(9, 50, LINES / 3, COLS / 3);
        box(main_win, 0, 0);

        mvwprintw(main_win, 1, 1, "Select an option:");
        mvwprintw(main_win, 2, 1, "1. Registration");
        mvwprintw(main_win, 3, 1, "2. Login");
        mvwprintw(main_win, 7, 1, "Press 'q' to exit");
        wrefresh(main_win);

        int choice = wgetch(main_win);
        if (choice == '1')
        {
            wclear(main_win);
            delwin(main_win);
            return 1; // Выбор регистрации
        }
        else if (choice == '2')
        {
            wclear(main_win);
            delwin(main_win);
            return 2; // Выбор логина
        }
        else if (choice == 'q' || choice == 'Q')
        {
            wclear(main_win);
            delwin(main_win);
            clean_up();
            exit(0); // Выход из программы
        }
    }
}