
#include <stdio.h>
#include "../sncc.h"

int main() {
  vector* tokenss = lexer();
  tokenstream* ts = new_tokenstream(tokenss);
  funcdecl fdecl = parse_funcdecl(ts);

  printf("%s ", fdecl.fdecl);
  for (int i=0; i<paramtypelist_len(fdecl.argdecls); i++) {
    char* pt = paramtypelist_get(fdecl.argdecls, i);
    printf(" %s", pt);
  }
}
