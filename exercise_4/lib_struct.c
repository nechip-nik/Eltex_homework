#include "lib_struct.h"
#include <stdio.h>
#include <string.h>

int subscriber_search(char *str, char *sub_str) {
  if ((strlen(str) == strlen(sub_str)) && strstr(str, sub_str)) {
    return 1;
  }
  return 0;
}

void adding_a_subscriber(char *i) {}