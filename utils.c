#include <stdio.h>
#include <stdbool.h>

void assert(bool b) {
  if (!b) {
    fprintf(stderr, "Assertion faled");
    exit(1);
  }
}
void error(char* fmt) {
  fprintf(stderr, fmt);
  exit(1);
}
void warning(char* fmt) {
  fprintf(stderr, "warning: ");
  fprintf(stderr, fmt);
  exit(1);
}
