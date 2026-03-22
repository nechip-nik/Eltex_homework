#include "../include/messanger.h"
void add_message(const char *username, const char *message)
{
    strncpy(chatroom.messages[chatroom.message_count].username, username,
            MAX_NAME_LEN);
    strncpy(chatroom.messages[chatroom.message_count].message, message,
            MAX_MESSAGE_LEN);
    chatroom.message_count++;
}