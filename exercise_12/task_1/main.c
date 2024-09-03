#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/wait.h>

int main() {
    int pipefd[2]; 
    pid_t pid;
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }
    if (pid != 0){
        close(pipefd[0]);
        const char message[4] = "Hi!"; 
        write(pipefd[1], message, strlen(message));
        close(pipefd[1]);
        wait(NULL);
        printf("Родительский процесс завершен.\n");
    }
    else
    {
        char buffer[256];
        close(pipefd[1]);
        int size = read(pipefd[0], buffer, sizeof(buffer));
        if(size == -1) {
        perror("read");
        exit(EXIT_FAILURE);
        }
        buffer[size] = '\0';
        printf("%s\n",buffer);
        close(pipefd[0]);
        exit(EXIT_SUCCESS);

    }
    return 0;
    
}