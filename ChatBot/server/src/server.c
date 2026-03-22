#include "../include/lib_client_handler.h"
#include "../include/lib_network.h"
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
int main()
{
    int server_socket = create_server_socket(34543);
    printf("Server listening on port 34543\n");
    delete_user_history_files();

    while (1)
    {
        struct sockaddr_in client_addr;
        int client_socket = accept_client(server_socket, &client_addr);

        if (client_socket < 0)
        {
            continue;
        }

        if (client_count >= MAX_COUNT_USERS)
        {
            printf("Maximum clients reached. Connection refused.\n");
            close(client_socket);
            continue;
        }

        pthread_mutex_lock(&clients_mutex);

        clients[client_count].socket = client_socket;
        strncpy(clients[client_count].name, "Anonymous", MAX_NAME_LENGTH - 1);
        clients[client_count].name[MAX_NAME_LENGTH - 1] = '\0';
        strncpy(clients[client_count].password, "", MAX_PASSWORD_LENGTH - 1);
        clients[client_count].password[MAX_PASSWORD_LENGTH - 1] = '\0';
        clients[client_count].active = ACTION_ANONYMOUS;

        client_count++;
        pthread_mutex_unlock(&clients_mutex);

        pthread_t tid;
        pthread_create(&tid, NULL, handle_client, &clients[client_count - 1]);
        pthread_detach(tid);

        printf("New client connected\n");

        for (int i = 0; i < client_count; i++)
        {
            printf("Client %d: name=%s, password=%s, active=%d\n", i,
                   clients[i].name, clients[i].password, clients[i].active);
        }
        printf("END\n");
    }

    close(server_socket);
    return 0;
}