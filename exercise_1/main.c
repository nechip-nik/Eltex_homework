#include "lib_bin.h"
#include <stdio.h>

int main() {
  char a;
  printf(" 1. Вывести двоичное представление целого положительного числа, "
         "используя битовые операции (число вводится с клавиатуры).\n 2. "
         "Вывести двоичное представление целого отрицательного числа, "
         "используя битовые операции (число вводится с клавиатуры).\n 3. Найти "
         "количество единиц в двоичном представлении целого "
         "положительного числа (число вводится с клавиатуры).\n 4. Поменять в "
         "целом положительном числе (типа int) значение третьего "
         "байта на введенное пользователем число (изначальное число также "
         "вводится с клавиатуры).\n -------------------------\n Напишите цифру "
         "соответствующего задания\n");

  scanf("%c", &a);

  switch (a) {
  case '1':
    binary();
    break;
  case '2':
    negativeBinary();
    break;
  case '3':
    countBinary();
    break;
  case '4':
    changeByte();
    break;

  default:
    break;
  }
  return 0;
}