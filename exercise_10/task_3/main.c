#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

#define MAX_INPUT_SIZE 1024
#define MAX_ARGS 64

void bash_analogue(char *input) {
  char *args[MAX_ARGS];
  int arg_count = 0;

  char *token = strtok(input, " \t\n");
  while (token != NULL && arg_count < MAX_ARGS - 1) {
    args[arg_count++] = token;
    token = strtok(NULL, " \t\n");
  }
  args[arg_count] = NULL;

  if (arg_count == 0) {
    return;
  }

  if (strcmp(args[0], "exit") == 0) {
    exit(0);
  }

  pid_t pid = fork();
  if (pid == -1) {
    perror("fork");
    exit(1);
  } else if (pid == 0) {

    execvp(args[0], args);
    perror("execvp");
    exit(1);
  } else {

    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status)) {
      printf("Процесс завершился с кодом %d\n", WEXITSTATUS(status));
    } else if (WIFSIGNALED(status)) {
      printf("Процесс завершился по сигналу %d\n", WTERMSIG(status));
    }
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