#include "lib_struct.h"
#include <stdio.h>
#include <string.h>

int main() {
  Abonent phone_book[100];
  memset(phone_book, 0, sizeof(phone_book));
  char a;
  while (1) {
    printf("\n\n1. Добавить абонента.\n");
    printf("2. Удалить абонента.\n");
    printf("3. Поиск абонентов по имени.\n");
    printf("4. Вывод всех записей.\n");
    printf("5. Выход.\n");
    scanf(" %c", &a);
    switch (a) {
    case '1':
      adding_a_subscriber(phone_book);
      break;
    case '2':
      delete_abonent(phone_book);
      break;
    case '3':
      find_abonent(phone_book);
      break;
    case '4':
      print_abonent(phone_book);

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