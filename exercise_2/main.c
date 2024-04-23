#include "lib_array.h"
#include <stdio.h>

int main() {
  char a;
  printf("1. Вывести квадратную матрицу по заданному N.\n");
  printf("2. Вывести заданный массив размером N в обратном порядке.\n");
  printf("3. Заполнить верхний треугольник матрицы а нижний 0.\n");
  printf("4. Заполнить матрицу числами от 1 до N^2 уликой\n");
  printf("-------------------------\n");
  printf("Напишите цифру соответствующего задания\n");

  scanf("%c", &a);

  switch (a) {
  case '1':
    printf("\n\n");
    array();
    break;
  case '2':
    printf("\n\n");
    reverse();
    break;
  case '3':
    printf("\n\n");
    triangle();
    break;
  case '4':
    printf("\n\n");
    printSpiral();
    break;

  default:
    break;
  }
  return 0;
}