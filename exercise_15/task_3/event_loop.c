#include <signal.h>
#include <stdio.h>
#include <unistd.h>

int main() {
  sigset_t mask;
  int signum;

  sigemptyset(&mask);
  sigaddset(&mask, SIGUSR1);

  if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
    perror("sigprocmask");
    return 1;
  }

  printf("SIGUSR1 заблокирован. Ожидание сигнала...\n");

  while (1) {
    if (sigwait(&mask, &signum) != 0) {
      perror("sigwait");
      return 1;
    }

    if (signum == SIGUSR1) {
      printf("Получен сигнал SIGUSR1\n");
    }
  }

  return 0;
}