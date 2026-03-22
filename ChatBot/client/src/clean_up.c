#include "../include/messanger.h"
void clean_up()
{
    endwin();
    for (int i = 0; i < chatroom.num_clients; i++)
    {
        if (strcmp(chatroom.clients[i], current_user) == 0)
        {
            for (int j = i; j < chatroom.num_clients - 1; j++)
            {
                strcpy(chatroom.clients[j], chatroom.clients[j + 1]);
            }
            chatroom.num_clients--;
            break;
        }
    }
    exit(0);
}