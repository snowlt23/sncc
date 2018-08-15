#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct _vector {
  void** data;
  int cap;
  int len;
} vector;

typedef struct _varinfo {
  struct _typenode* typ;
  int offset;
} varinfo;

typedef struct _strlitinfo {
  char* label;
  char* strval;
} strlitinfo;

typedef struct _mappair {
  char* name;
  void* value;
} mappair;
typedef struct _map {
  vector* vector;
} map;

// vector.c
vector* new_vector_cap(int cap);
vector* new_vector();
void* vector_get(vector* v, int index);
void vector_set(vector* v, int index, void* elem);
void vector_push(vector* v, void* elem);

// map.c
map* new_map_cap(int cap);
map* new_map();
void* map_get(map* m, char* name);
void map_insert(map* m, char* name, void* value);
