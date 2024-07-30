#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  pid_t pid;
  int status;

  pid = fork();

  if (pid < 0) {
    perror("fork");
    exit(EXIT_FAILURE);
  } else if (pid == 0) {
    printf("Дочерний процесс: PID = %d, PPID = %d\n", getpid(), getppid());
    exit(EXIT_SUCCESS);
  } else {
    printf("Родительский процесс: PID = %d, PPID = %d\n", getpid(), getppid());
    wait(&status);
    if (WIFEXITED(status)) {
      printf("Дочерний процесс завершился с кодом %d\n", WEXITSTATUS(status));
    }
  }

  return 0;
}