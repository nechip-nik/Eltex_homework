#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main(int argc, char *argv[]) {
  if (argc != 2) {
    fprintf(stderr, "Использование: %s <PID>\n", argv[0]);
    return 1;
  }

  pid_t pid = atoi(argv[1]);

  if (kill(pid, SIGINT) == -1) {
    perror("kill");
    return 1;
  }

  printf("Сигнал SIGUSR1 отправлен процессу с PID %d\n", pid);

  return 0;
}