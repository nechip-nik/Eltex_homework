#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

int main() {
  pid_t pid;

  pid = fork();

  if (pid < 0) {
    perror("fork");
    exit(EXIT_FAILURE);
  } else if (pid == 0) {
    printf("Дочерний процесс: PID = %d, PPID = %d\n", getpid(), getppid());
    exit(EXIT_SUCCESS);
  } else {
    int status;
    printf("Родительский процесс: PID = %d, PPID = %d\n", getpid(), getppid());
    waitpid(pid,&status,0);
    if (WIFEXITED(status)) {
      printf("Дочерний процесс завершился с кодом %d\n", WEXITSTATUS(status));
    }
  }
}