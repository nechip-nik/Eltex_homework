#include "lib_pointer.h"
#include <stdio.h>

int main() {
  char a;
  printf(
      "1. Поменять в целом положительном числе (типа int) значение третьего\n");
  printf("2. Изменить только одну строку.\n");
  printf("3. Вывод элементов массива на консоль.\n");
  printf("4. Поиск подстроки.\n");
  printf("-------------------------\n");
  printf("Напишите цифру соответствующего задания\n");

  scanf("%c", &a);

  switch (a) {
  case '1':
    printf("\n\n");
    changeByte();
    break;
  case '2':
    printf("\n\n");
    correctPointer();
    break;
  case '3':
    printf("\n\n");
    pointerArray();
    break;
  case '4':
    printf("\n\n");
    findSubstring();
    break;

  default:
    break;
  }
  return 0;
}