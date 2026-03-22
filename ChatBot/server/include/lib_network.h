#ifndef LIB_NETWORK_H
#define LIB_NETWORK_H

#include <arpa/inet.h>
#include <pthread.h>
#include <unistd.h>

#define BUFFER_SIZE 1024

int create_server_socket(int port);
int accept_client(int server_socket, struct sockaddr_in *client_addr);
void send_message(int client_socket, const char *message);

#endif