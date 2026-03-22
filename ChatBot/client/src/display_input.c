#include "../include/messanger.h"

void display_input()
{
    werase(input_win);
    wattron(input_win, COLOR_PAIR(4));
    mvwprintw(input_win, 1, 1, "%s", input_message);
    wattroff(input_win, COLOR_PAIR(4));
    wrefresh(input_win);
}