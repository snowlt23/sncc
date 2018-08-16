typedef void FILE;

#define EXIT_FAILURE 1

extern FILE* stdin;
extern FILE* stderr;
extern char EOF;

FILE* fopen(char* filename, char* mode);
void fclose(FILE* fp);
char getc(FILE* fp);
void ungetc(char c, FILE* fp);
char fgetc(FILE* fp);
void putchar(char c);
void printf(char* fmt);
void fprintf(FILE* f, char* fmt);
void sprintf(char* s, char* fmt);
void snprintf(char* s, int n, char* fmt);
void exit(int code);
