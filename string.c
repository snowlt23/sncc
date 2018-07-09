#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include "sncc.h"

char* string_copy(char* s) {
  char* copied = (char*)malloc((strlen(s)+1)*sizeof(char));
  strcpy(copied, s);
  return copied;
}
