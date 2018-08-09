
#include <stdio.h>
#include "../vector.c"
#include "../map.c"

int main() {
  map* m = new_map();
  map_insert(m, "a", (mapelem){NULL, 1});
  map_insert(m, "b", (mapelem){NULL, 2});
  map_insert(m, "c", (mapelem){NULL, 3});
  map_insert(m, "d", (mapelem){NULL, 4});

  assert(1 == map_get(m, "a").pos);
  assert(2 == map_get(m, "b").pos);
  assert(3 == map_get(m, "c").pos);
  assert(4 == map_get(m, "d").pos);
  map_insert(m, "a", (mapelem){NULL, 9});
  assert(9 == map_get(m, "a").pos);

  return 0;
}
