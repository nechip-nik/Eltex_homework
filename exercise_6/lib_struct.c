#include "lib_struct.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int subscriber_search(char *str, char *sub_str) {
  if ((strlen(str) == strlen(sub_str)) && strstr(str, sub_str)) {
    return 1;
  }
  return 0;
}

void adding_a_subscriber(Abonent **phone_book, int *phone_book_size) {
  (*phone_book_size)++;
  *phone_book = realloc(*phone_book, *phone_book_size * sizeof(Abonent));
  if (*phone_book == NULL) {
    printf("Ошибка выделения памяти.\n");
    exit(1);
  }

  printf("\n\nВведите имя\n");
  scanf("%s", (*phone_book)[*phone_book_size - 1].name);
  printf("\n\nВведите фамилию\n");
  scanf("%s", (*phone_book)[*phone_book_size - 1].second_name);
  printf("\n\nВведите телефон\n");
  scanf("%s", (*phone_book)[*phone_book_size - 1].tel);
}

void print_abonent(Abonent *phone_book, int phone_book_size) {
  printf("\n\n");
  for (int i = 0; i < phone_book_size; i++) {

    printf("%d: %s %s %s\n", i + 1, phone_book[i].name,
           phone_book[i].second_name, phone_book[i].tel);
  }
}
void delete_abonent(Abonent **phone_book, int *phone_book_size) {
  printf("\n\nВведите имя\n");
  char name[10];
  scanf("%s", name);
  for (int i = 0; i < *phone_book_size; i++) {
    if (subscriber_search((*phone_book)[i].name, name)) {
      // Перемещаем удаляемый элемент в конец массива
      Abonent temp = (*phone_book)[i];
      for (int j = i; j < *phone_book_size - 1; j++) {
        (*phone_book)[j] = (*phone_book)[j + 1];
      }
      (*phone_book)[*phone_book_size - 1] = temp;

      // Уменьшаем размер массива
      (*phone_book_size)--;
      *phone_book = realloc(*phone_book, *phone_book_size * sizeof(Abonent));
      if (*phone_book == NULL) {
        printf("Ошибка выделения памяти.\n");
        exit(1);
      }
      printf("Абонент удален.\n");
      break;
    }
  }
}
void find_abonent(Abonent *phone_book, int phone_book_size) {
  printf("\n\n");
  char name[10];
  scanf("%s", name);
  for (int i = 0; i < phone_book_size; i++) {
    if (subscriber_search(phone_book[i].name, name)) {
      printf("%d: %s %s %s\n", i + 1, phone_book[i].name,
             phone_book[i].second_name, phone_book[i].tel);
    } else {
      printf("Такого абонента не существует.\n");
    }
  }
}