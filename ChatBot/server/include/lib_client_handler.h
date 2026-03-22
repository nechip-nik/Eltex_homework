#ifndef LIB_CLIENT_HANDLER_H
#define LIB_CLIENT_HANDLER_H

#include "lib_network.h"
#include "lib_user.h"

#define MAX_BANNED_IPS 100
void *handle_client(void *arg);

extern User clients[MAX_COUNT_USERS];
extern int client_count;
extern pthread_mutex_t clients_mutex;

#endif