typedef void FILE;

#define EOF (-1)
#define EXIT_FAILURE 1

extern FILE* stderr;

FILE* fopen(char* filename, char* mode);
char fgetc(FILE* fp);
void putchar(char c);
void printf(char* fmt);
void fprintf(FILE* f, char* fmt);
void sprintf(char* s, char* fmt);
void exit(int code);
