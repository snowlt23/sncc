
#include <stdio.h>
#include "../sncc.h"

int main() {
  vector* tokenss = lexer();
  tokenstream* ts = new_tokenstream(tokenss);
  funcdecl fdecl = parse_funcdecl(ts);

  printf("%s ", fdecl.fdecl->name);
  for (int i=0; i<fdecl.argdecls->len; i++) {
    paramtype* pt = vector_get(fdecl.argdecls, i);
    printf(" %s", pt->name);
  }
}
