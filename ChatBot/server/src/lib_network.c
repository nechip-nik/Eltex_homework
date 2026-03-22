#include "../include/lib_network.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int create_server_socket(int port)
{
    int server_socket = socket(AF_INET, SOCK_STREAM, 0);
    if (server_socket < 0)
    {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    struct sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port = htons(port);

    if (bind(server_socket, (struct sockaddr *)&server_addr,
             sizeof(server_addr)) < 0)
    {
        perror("Bind failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    if (listen(server_socket, 10) < 0)
    {
        perror("Listen failed");
        close(server_socket);
        exit(EXIT_FAILURE);
    }

    return server_socket;
}

int accept_client(int server_socket, struct sockaddr_in *client_addr)
{
    socklen_t addr_len = sizeof(*client_addr);
    int client_socket =
        accept(server_socket, (struct sockaddr *)client_addr, &addr_len);

    if (client_socket < 0)
    {
        perror("Accept failed");
        return -1;
    }

    return client_socket;
}

void send_message(int client_socket, const char *message)
{
    send(client_socket, message, strlen(message), 0);
}