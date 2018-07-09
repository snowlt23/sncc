
#include <stdio.h>
#include "../sncc.h"

int main() {
  vector* tokenss = lexer();
  tokenstream* ts = new_tokenstream(tokenss);
  funcdecl fdecl = parse_funcdecl(ts);

  printf("%s %s", fdecl.returntype.name, fdecl.fdecl.name);
  for (int i=0; i<paramtypelist_len(fdecl.argdecls); i++) {
    paramtype pt = paramtypelist_get(fdecl.argdecls, i);
    printf(" %s %s", pt.type.name, pt.decl.name);
  }
}
