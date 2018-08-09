#include <stdlib.h>

int num9() {
  return 9;
}

int add5(int x) {
  return x + 5;
}

int adiv(int a, int b) {
  return a / b;
}

int multidiv6(int a1, int a2, int a3, int a4, int a5, int a6) {
  return a1 / a2 / a3 / a4 / a5 / a6;
}

int multidiv6_2(int a1, int a2, int a3, int a4, int a5, int a6, int a7, int a8) {
  return a1 / a2 / a3 / a4 / a5 / a6 / (a7 / a8);
}

void alloc4(int** p, int a, int b, int c, int d) {
  *p = malloc(sizeof(int)*4);
  (*p)[0] = a;
  (*p)[1] = b;
  (*p)[2] = c;
  (*p)[3] = d;
}
