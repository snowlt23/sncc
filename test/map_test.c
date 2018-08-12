
#include <stdio.h>
#include "../vector.c"
#include "../map.c"

int* pint(int x) {
  int* p = malloc(sizeof(int));
  *p = x;
  return p;
}

int main() {
  map* m = new_map();
  map_insert(m, "a", pint(1));
  map_insert(m, "b", pint(2));
  map_insert(m, "c", pint(3));
  map_insert(m, "d", pint(4));

  assert(1 == *(int*)map_get(m, "a"));
  assert(2 == *(int*)map_get(m, "b"));
  assert(3 == *(int*)map_get(m, "c"));
  assert(4 == *(int*)map_get(m, "d"));
  map_insert(m, "a", pint(9));
  assert(9 == *(int*)map_get(m, "a"));

  return 0;
}
