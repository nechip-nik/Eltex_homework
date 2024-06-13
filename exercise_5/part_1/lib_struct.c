#include "lib_struct.h"
#include <stdio.h>
#include <string.h>
Abonent phone_book[100];
int subscriber_search(char *str, char *sub_str) {
  if ((strlen(str) == strlen(sub_str)) && strstr(str, sub_str)) {
    return 1;
  }
  return 0;
}

void adding_a_subscriber() {
  for (int i = 0; i <= 100; i++) {
    if (i == 100) {
      printf("Справочник переполнен, больше нельзя побавить абонентов.");
      break;
    }

    if (strlen(phone_book[i].name) == 0) {
      printf("\n\nВведите имя\n");
      scanf("%s", phone_book[i].name);
      printf("\n\nВведите фамилию\n");
      scanf("%s", phone_book[i].second_name);
      printf("\n\nВведите телефон\n");
      scanf("%s", phone_book[i].tel);
      break;
    }
  }
}
void print_abonent() {
  printf("\n\n");
  for (int i = 0; i < 100; i++) {

    printf("%d: %s %s %s\n", i + 1, phone_book[i].name,
           phone_book[i].second_name, phone_book[i].tel);
  }
}
void delete_abonent() {
  printf("\n\nВведите имя\n");
  char name[10];
  scanf("%s", name);
  for (int i = 0; i < 100; i++) {

    if (subscriber_search(phone_book[i].name, name)) {
      memset(&phone_book[i], 0, sizeof(Abonent));
      printf("Абонент удален.\n");
      break;
    }
  }
}
void find_abonent() {
  printf("\n\n");
  char name[10];
  scanf("%s", name);
  for (int i = 0; i < 100; i++) {
    if (subscriber_search(phone_book[i].name, name)) {
      printf("%d: %s %s %s\n", i + 1, phone_book[i].name,
             phone_book[i].second_name, phone_book[i].tel);
    }
  }
}