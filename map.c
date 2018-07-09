#include <stdlib.h>
#include <string.h>
#include "sncc.h"

map* new_map_cap(int cap) {
  map* m = (map*)malloc(sizeof(map));
  m->vector = new_vector_cap(cap);
  return m;
}

map* new_map() {
  return new_map_cap(256);
}

mappair* new_mappair(char* name, int pos) {
  mappair* mp = (mappair*)malloc(sizeof(mappair));
  mp->name = name;
  mp->pos = pos;
  return mp;
}

int map_get(map* m, char* name) {
  for (int i=0; i<m->vector->len; i++) {
    mappair* mp = (mappair*)vector_get(m->vector, i);
    if (strcmp(mp->name, name) == 0) {
      return mp->pos;
    }
  }
}
void map_insert(map* m, char* name, int pos) {
  for (int i=0; i<m->vector->len; i++) {
    mappair* mp = (mappair*)vector_get(m->vector, i);
    if (strcmp(mp->name, name) == 0) {
      mp->pos = pos;
      return;
    }
  }
  vector_push(m->vector, new_mappair(name, pos));
}
