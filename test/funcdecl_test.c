
#include <stdio.h>
#include "../sncc.h"

int main() {
  vector* tokenss = lexer();
  tokenstream* ts = new_tokenstream(tokenss);
  toplevel top = parse_toplevel(ts);

  printf("%s ", top.fdecl.fdecl->name);
  for (int i=0; i<top.fdecl.argdecls->len; i++) {
    paramtype* pt = vector_get(top.fdecl.argdecls, i);
    printf(" %s", pt->name);
  }
}
