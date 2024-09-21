#include <signal.h>
#include <stdio.h>
#include <unistd.h>

int main() {
  sigset_t mask;

  sigemptyset(&mask);
  sigaddset(&mask, SIGINT);

  if (sigprocmask(SIG_BLOCK, &mask, NULL) == -1) {
    perror("sigprocmask");
    return 1;
  }

  printf("SIGINT заблокирован. Ожидание сигнала...\n");

  while (1) {
    sleep(1);
  }

  return 0;
}