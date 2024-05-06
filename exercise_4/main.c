#include "lib_struct.h"
#include <stdio.h>
#include <string.h>

int main() {

  Abonent book[100] = {0};
  char a;
  char name[10];
  while (1) {
    printf("\n\n1. Добавить абонента.\n");
    printf("2. Удалить абонента.\n");
    printf("3. Поиск абонентов по имени.\n");
    printf("4. Вывод всех записей.\n");
    printf("5. Выход.\n");
    scanf(" %c", &a);
    switch (a) {
    case '1':
      for (int i = 0; i <= 100; i++) {
        if (i == 100) {
          printf("Справочник переполнен, больше нельзя побавить абонентов.");
          break;
        }

        if (strlen(book[i].name) == 0) {
          printf("\n\nВведите имя\n");
          scanf("%s", book[i].name);
          printf("\n\nВведите фамилию\n");
          scanf("%s", book[i].second_name);
          printf("\n\nВведите телефон\n");
          scanf("%s", book[i].tel);
          break;
        }
      }
      break;
    case '2':
      printf("\n\nВведите имя\n");
      scanf("%s", name);
      for (int i = 0; i < 100; i++) {

        if (subscriber_search(book[i].name, name)) {
          memset(&book[i], 0, sizeof(Abonent));
          printf("Абонент удален.\n");
          break;
        }
      }
      break;
    case '3':
      printf("\n\n");
      scanf("%s", name);
      for (int i = 0; i < 100; i++) {
        if (subscriber_search(book[i].name, name)) {
          printf("%d: %s %s %s\n", i + 1, book[i].name, book[i].second_name,
                 book[i].tel);
        }
      }
      break;
    case '4':
      printf("\n\n");
      for (int i = 0; i < 100; i++) {

        printf("%d: %s %s %s\n", i + 1, book[i].name, book[i].second_name,
               book[i].tel);
      }
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