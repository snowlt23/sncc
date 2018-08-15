#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILENAME_LEN 255

int main(int argc, char **argv) {
  char filename[MAX_FILENAME_LEN]; 
 
  if (argc <= 1) {
    printf("Usage cat <filename> \n");
    return 0;
  }
 
  for (int i=1; i<argc; i++){
    strncpy(filename, argv[i], MAX_FILENAME_LEN);
 
    FILE* fp = fopen(filename, "r");
    if (fp == NULL) {
      printf("%s: No such file or directory\n", filename);
      return 1;
    }
 
    char c = fgetc(fp);
    char eof = EOF;
    while (c != eof) {
      putchar(c);
      c = fgetc(fp);
    }
    fclose(fp);
  }
  return 0;
}
