#include "../include/libcalc.h"
#include <stdio.h>

int main() {
  char a;
  int param_1, param_2;
  while (1) {
    param_1 = 0;
    param_2 = 0;
    printf("\n\n1. Сложение.\n");
    printf("2. Вычитание.\n");
    printf("3. Умножение.\n");
    printf("4. Деление.\n");
    printf("5. Выход.\n");
    scanf(" %c", &a);

    switch (a) {
    case '1':
      printf("\n\nВведите первое число.\n");
      scanf("%d", &param_1);
      printf("\n\nВведите второе число.\n");
      scanf("%d", &param_2);
      printf("Результат: %d\n", add(param_1, param_2));
      break;
    case '2':
      printf("\n\nВведите первое число.\n");
      scanf("%d", &param_1);
      printf("\n\nВведите второе число.\n");
      scanf("%d", &param_2);
      printf("Результат: %d\n", sub(param_1, param_2));
      break;
    case '3':
      printf("\n\nВведите первое число.\n");
      scanf("%d", &param_1);
      printf("\n\nВведите второе число.\n");
      scanf("%d", &param_2);
      printf("Результат: %d\n", mul(param_1, param_2));
      break;
    case '4':
      printf("\n\nВведите первое число.\n");
      scanf("%d", &param_1);
      printf("\n\nВведите второе число.\n");
      scanf("%d", &param_2);
      printf("Результат: %d\n", div(param_1, param_2));
      break;
    case '5':
      printf("Выход из программы.\n");

      return 0;
    default:
      break;
    }
    while (getchar() != '\n')
      ;
  }
}