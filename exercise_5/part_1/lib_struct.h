#ifndef LIB_STRUCT_H
#define LIB_STRUCT_H

typedef struct {
  char name[10];
  char second_name[10];
  char tel[10];
} Abonent;

int subscriber_search(char *str, char *sub_str);
void adding_a_subscriber();
void print_abonent();
void delete_abonent();
void find_abonent();

#endif