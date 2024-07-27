#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void reverse_print_file(const char *filename) {
  FILE *file = fopen(filename, "r");
  if (file == NULL) {
    perror("Failed to open file");
    exit(EXIT_FAILURE);
  }

  fseek(file, 0, SEEK_END);
  long file_size = ftell(file);

  for (long i = file_size - 1; i >= 0; i--) {
    fseek(file, i, SEEK_SET);
    char ch = fgetc(file);
    putchar(ch);
  }
  printf("\n");

  fclose(file);
}

int main() {
  const char *filename = "output.txt";
  const char *message = "String from file";

  FILE *file = fopen(filename, "w");
  if (file == NULL) {
    perror("Failed to open file for writing");
    return EXIT_FAILURE;
  }
  fprintf(file, "%s", message);
  fclose(file);

  reverse_print_file(filename);

  return 0;
}