#include <stdio.h>
#include "sncc.h"

int main() {
  vector* tokenss = lexer();
  tokenstream* ts = new_tokenstream(tokenss);

  emit_global("main");
  for (;;) {
    if (get_token(ts) == NULL) break;
    funcdecl fdecl = parse_funcdecl(ts);
    codegen_funcdecl(fdecl);
  }

  return 0;
}
