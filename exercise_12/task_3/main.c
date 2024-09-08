#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_INPUT_SIZE 1024
#define MAX_ARGS 64

void bash_analogue(char *input) {
  char *args[MAX_ARGS][MAX_ARGS];
  int arg_count[MAX_ARGS] = {0};
  int cmd_count = 0;

  char *token = strtok(input, "|");
  while (token != NULL && cmd_count < MAX_ARGS) {
    char *arg_token = strtok(token, " \t\n");
    while (arg_token != NULL && arg_count[cmd_count] < MAX_ARGS - 1) {
      args[cmd_count][arg_count[cmd_count]++] = arg_token;
      arg_token = strtok(NULL, " \t\n");
    }
    args[cmd_count][arg_count[cmd_count]] = NULL;
    cmd_count++;
    token = strtok(NULL, "|");
  }

  if (cmd_count == 0) {
    return;
  }

  if (strcmp(args[0][0], "exit") == 0) {
    exit(0);
  }

  int pipefd[2];
  pid_t pid;
  int status;

  for (int i = 0; i < cmd_count; i++) {
    if (i < cmd_count - 1) {
      if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(1);
      }
    }

    pid = fork();
    if (pid == -1) {
      perror("fork");
      exit(1);
    } else if (pid == 0) {
      if (i < cmd_count - 1) {
        if (dup2(pipefd[1], STDOUT_FILENO) == -1) {
          perror("dup2");
          exit(1);
        }
        close(pipefd[0]);
        close(pipefd[1]);
      }

      if (i > 0) {
        if (dup2(pipefd[0], STDIN_FILENO) == -1) {
          perror("dup2");
          exit(1);
        }
        close(pipefd[0]);
        close(pipefd[1]);
      }

      execvp(args[i][0], args[i]);
      perror("execvp");
      exit(1);
    } else {

      if (i > 0) {
        close(pipefd[0]);
      }
      if (i < cmd_count - 1) {
        close(pipefd[1]);
      }
      waitpid(pid, &status, 0);
    }
  }

  if (WIFEXITED(status)) {
    printf("Процесс завершился с кодом %d\n", WEXITSTATUS(status));
  } else if (WIFSIGNALED(status)) {
    printf("Процесс завершился по сигналу %d\n", WTERMSIG(status));
  }
}

int main() {
  char input[MAX_INPUT_SIZE];

  while (1) {
    printf("my_bash:=> ");
    if (fgets(input, MAX_INPUT_SIZE, stdin) == NULL) {
      perror("bash");
      break;
    }
    bash_analogue(input);
  }

  return 0;
}