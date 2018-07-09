
#include <stdio.h>
#include "../vector.c"
#include "../map.c"

int main() {
  map* m = new_map();
  map_insert(m, "a", 1);
  map_insert(m, "b", 2);
  map_insert(m, "c", 3);
  map_insert(m, "d", 4);
  printf("%d ", map_get(m, "a"));
  printf("%d ", map_get(m, "b"));
  printf("%d ", map_get(m, "c"));
  printf("%d ", map_get(m, "d"));
  map_insert(m, "a", 9);
  printf("%d ", map_get(m, "a"));
}
