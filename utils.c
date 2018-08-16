#include <stdio.h>
#include <stdbool.h>

void assert(bool b) {
  if (!b) {
    fprintf(stderr, "Assertion faled\n");
    exit(1);
  }
}

void error(char* fmt) {
  fprintf(stderr, fmt);
  fprintf(stderr, "\n");
  exit(1);
}
void error_s(char* fmt, char* s) {
  fprintf(stderr, fmt, s);
  fprintf(stderr, "\n");
  exit(1);
}
void error_ss(char* fmt, char* s1, char* s2) {
  fprintf(stderr, fmt, s1, s2);
  fprintf(stderr, "\n");
  exit(1);
}

void warning(char* fmt) {
  fprintf(stderr, "warning: ");
  fprintf(stderr, fmt);
  fprintf(stderr, "\n");
}
void warning_s(char* fmt, char* s) {
  fprintf(stderr, "warning: ");
  fprintf(stderr, fmt, s);
  fprintf(stderr, "\n");
}
