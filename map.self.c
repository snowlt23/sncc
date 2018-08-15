
typedef struct _vector {
  void** data;
  int cap;
  int len;
} vector;

typedef struct _mappair {
  char* name;
  void* value;
} mappair;

typedef struct _map {
  vector* vector;
} map;

void* malloc(int size);
int strcmp(char* a, char* b);

map* new_map_cap(int cap) {
  map* m = malloc(sizeof(map));
  m->vector = new_vector_cap(cap);
  return m;
}

map* new_map() {
  return new_map_cap(256);
}

mappair* new_mappair(char* name, void* value) {
  mappair* mp = malloc(sizeof(mappair));
  mp->name = name;
  mp->value = value;
  return mp;
}

void* map_get(map* m, char* name) {
  for (int i=0; i<m->vector->len; i++) {
    mappair* mp = vector_get(m->vector, i);
    if (strcmp(mp->name, name) == 0) {
      return mp->value;
    }
  }
  return 0;
}

void map_insert(map* m, char* name, void* value) {
  for (int i=0; i <m->vector->len; i++) {
    mappair* mp = vector_get(m->vector, i);
    if (strcmp(mp->name, name) == 0) {
      mp->value = value;
      return;
    }
  }
  vector_push(m->vector, new_mappair(name, value));
}
