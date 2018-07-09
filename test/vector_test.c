#include <stdio.h>
#include <stdlib.h>
#include "../vector.c"

int* pint(int x) {
  int* p = (int*)malloc(sizeof(int));
  *p = x;
  return p;
}

int main() {
  vector* v = new_vector_cap(1);
  vector_push(v, pint(1));
  vector_push(v, pint(2));
  vector_push(v, pint(3));
  vector_push(v, pint(4));
  vector_push(v, pint(5));

  for (int i=0; i<v->len; i++) {
    printf("%d", *(int*)vector_get(v, i));
  }
}
