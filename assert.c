#include <stdbool.h>

void assert(bool b) {
  if (!b) {
    printf("Assertion faled");
    exit(1);
  }
}

