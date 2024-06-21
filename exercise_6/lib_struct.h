#ifndef LIB_STRUCT_H
#define LIB_STRUCT_H

typedef struct {
  char name[10];
  char second_name[10];
  char tel[10];
} Abonent;

int subscriber_search(char *str, char *sub_str);
void adding_a_subscriber(Abonent **phone_book, int *phone_book_size);
void print_abonent(Abonent *phone_book, int phone_book_size);
void delete_abonent(Abonent **phone_book, int *phone_book_size);
void find_abonent(Abonent *phone_book, int phone_book_size);
#endif