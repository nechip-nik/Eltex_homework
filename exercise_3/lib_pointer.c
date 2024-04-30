#include "lib_pointer.h"
#include <stdio.h>
#include <string.h>

void changeByte() {
  int number;
  unsigned char newByte;

  printf("Введите исходное целое положительное число: ");
  scanf("%d", &number);

  printf("Введите новое значение для третьего байта (от 0 до 255): ");
  scanf("%hhu", &newByte);
  unsigned char *thirdByte = (unsigned char *)&number + 2;

  *thirdByte = newByte;

  printf("Новое число с измененным третьим байтом: %d\n", number);
}

void correctPointer() {
  float x = 5.0;
  printf("x = %f, ", x);
  float y = 6.0;
  printf("y = %f\n", y);
  float *xp = &y; // TODO: отредактируйте эту строку, и только правую часть
  float *yp = &y;
  printf("Результат: %f\n", *xp + *yp);
}

void pointerArray() {
  int array[10] = {1, 2, 3, 4, 5, 6, 7, 8, 9, 10};
  int *ptr = array;
  for (int i = 0; i < 10; i++) {
    printf("%d ", *ptr);
    ptr++;
  }
  printf("\n");
}

char *findSubstring() {
  char str[10];
  char sub[10];
  char *result = NULL;
  printf("Введите строку\n");
  scanf("%s", str);
  printf("Введите подстроку\n");
  scanf("%s", sub);
  char *ptr_str = str;
  char *ptr_sub = NULL;
  while (*ptr_str != '\0') {
    ptr_sub = sub;
    if (*ptr_str == *ptr_sub) {
      result = ptr_str;
      while (*ptr_str != '\0' && *ptr_sub != '\0' && *ptr_str == *ptr_sub) {
        ptr_str++;
        ptr_sub++;
      }
      if (*ptr_str == '\0' || *ptr_sub == '\0') {
        printf("Указатель на первый символ : %c\n", *result);
        return result;
      }

    } else {
      ptr_str++;
    }
  }
  return NULL;
}