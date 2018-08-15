#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_FILE_NAME_CHARS 255

int main(int argc, char **argv) {
  char file_name[MAX_FILE_NAME_CHARS]; 
 
  if (argc <= 1) {
    printf("Usage cat <filename> \n");
    return 0;
  }
 
  for (int i=1; i<argc; i++){
    strncpy(file_name, argv[i], MAX_FILE_NAME_CHARS);
 
    FILE* fp = fopen(file_name, "r");
    if (fp == NULL) {
       printf("%s: No such file or directory\n", file_name);
       return 1;
    }
 
    while (!feof(fp)) {
      char c = fgetc(fp);
      putchar(c);
    }
    fclose(fp);
  }
  return 0;
}
