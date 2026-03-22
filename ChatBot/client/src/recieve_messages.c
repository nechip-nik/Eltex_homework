#include "../include/messanger.h"
void *receive_messages(void *arg)
{
    int prev_message_count = chatroom.message_count;
    int prev_user_count = chatroom.num_clients;
    while (1)
    {
        if (chatroom.message_count != prev_message_count)
        {
            prev_message_count = chatroom.message_count;
            display_messages();
        }
        sleep(1);
    }
    return NULL;
}