#ifndef LIB_STRUCT_H
#define LIB_STRUCT_H

typedef struct {
  char name[10];
  char second_name[10];
  char tel[10];
} Abonent;

int subscriber_search(char *str, char *sub_str);

#endif