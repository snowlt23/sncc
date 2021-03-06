
#include <stdio.h>
#include <assert.h>
#include "../sncc.h"

int main() {
  init_lexer();
  init_parser();
  vector* tokenss = lexer();
  tokenstream* ts = new_tokenstream(tokenss);
  toplevel* top = parse_toplevel(ts);

  if (top->kind == TOP_FUNCDECL) {
    printf("%s ", top->fdecl->name);
    for (int i=0; i<top->argdecls->len; i++) {
      paramtype* pt = vector_get(top->argdecls, i);
      printf(" %s", pt->name);
    }
  } else if (top->kind == TOP_STRUCT) {
    printf("%s ", top->structtype->tname);
    for (int i=0; i<top->structtype->fields->len; i++) {
      paramtype* pt = vector_get(top->structtype->fields, i);
      printf(" %s", pt->name);
    }
  } else {
    assert(false);
  }
}
