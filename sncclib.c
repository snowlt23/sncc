#include <stdlib.h>

typedef struct _ptrint {
  int kind;
  void* fdecl;
  void* argdecls;
  void* body;
  int stacksize;
  void* vdecl;
  void* vinit;
  void* structtype;
} ptrint;

struct incom {
  int x;
  struct incom* next;
};

int addpp(int* a, int* b) {
  return *a + *b;
}
int addic(int a, char b) {
  return a + b;
}
int* padd(int a, char b) {
  int* p = malloc(sizeof(int));
  *p = a+b;
  return p;
}

int pi_stacksize(ptrint* pi) {
  return pi->stacksize;
}

struct incom* new_incom() {
  struct incom* ic = malloc(sizeof(struct incom));
  ic->next = malloc(sizeof(struct incom));
  ic->next->x = 9;
  return ic;
}

