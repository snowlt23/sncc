
#include <stdio.h>
#include "../vector.c"
#include "../map.c"

int main() {
  map* m = new_map();
  map_insert(m, "a", 1);
  map_insert(m, "b", 2);
  map_insert(m, "c", 3);
  map_insert(m, "d", 4);

  assert(1 == map_get(m, "a"));
  assert(2 == map_get(m, "b"));
  assert(3 == map_get(m, "c"));
  assert(4 == map_get(m, "d"));
  map_insert(m, "a", 9);
  assert(9 == map_get(m, "a"));

  return 0;
}
