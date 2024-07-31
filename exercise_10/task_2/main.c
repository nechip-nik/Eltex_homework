#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

void process_1();
void process_2();
void process_3();
void process_4();
void process_5();

int main() {
  pid_t pid1, pid2;

  pid1 = fork();
  if (pid1 < 0) {
    perror("fork");
    exit(EXIT_FAILURE);
  } else if (pid1 == 0) {
    process_1();
    exit(EXIT_SUCCESS);
  }
  pid2 = fork();
  if (pid2 < 0) {
    perror("fork");
    exit(EXIT_FAILURE);
  } else if (pid2 == 0) {
    process_2();
    exit(EXIT_SUCCESS);
  }
  waitpid(pid1, NULL, 0);
  waitpid(pid2, NULL, 0);
  printf("Родительский процесс: PID = %d, PPID = %d\n", getpid(), getppid());
}

void process_1() {
  pid_t pid3, pid4;

  pid3 = fork();
  if (pid3 < 0) {
    perror("fork");
    exit(EXIT_FAILURE);
  } else if (pid3 == 0) {
    process_3();
    exit(EXIT_SUCCESS);
  }
  pid4 = fork();
  if (pid4 < 0) {
    perror("fork");
    exit(EXIT_FAILURE);
  } else if (pid4 == 0) {
    process_4();
    exit(EXIT_SUCCESS);
  }
  waitpid(pid3, NULL, 0);
  waitpid(pid4, NULL, 0);
  printf("Дочерний процесс 1: PID = %d, PPID = %d\n", getpid(), getppid());
}
void process_2() {
  pid_t pid5;

  pid5 = fork();
  if (pid5 < 0) {
    perror("fork");
    exit(EXIT_FAILURE);
  } else if (pid5 == 0) {
    process_5();
    exit(EXIT_SUCCESS);
  }
  waitpid(pid5, NULL, 0);
  printf("Дочерний процесс 2: PID = %d, PPID = %d\n", getpid(), getppid());
}
void process_3() {
  printf("Дочерний процесс 3: PID = %d, PPID = %d\n", getpid(), getppid());
}

void process_4() {
  printf("Дочерний процесс 4: PID = %d, PPID = %d\n", getpid(), getppid());
}

void process_5() {
  printf("Дочерний процесс 5: PID = %d, PPID = %d\n", getpid(), getppid());
}