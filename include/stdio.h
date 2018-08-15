typedef void FILE;

#define EXIT_FAILURE 1

FILE* fopen(char* filename, char* mode);
char fgetc(FILE* fp);
void putchar(char c);
void printf(char* fmt);
void fprintf(FILE* f, char* fmt);
void sprintf(char* s, char* fmt);
void exit(int code);
