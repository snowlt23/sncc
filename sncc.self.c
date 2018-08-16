#include <stdio.h>
#include "sncc.self.h"

int main() {
  init_lexer();
  init_parser();
  vector* tokenss = lexer();
  tokenstream* ts = new_tokenstream(tokenss);

  init_semantic();
  for (;;) {
    if (get_token(ts) == NULL) break;
    toplevel* top = parse_toplevel(ts);
    fprintf(stderr, "%d\n", top->kind);
    fprintf(stderr, "%s\n", top->fdecl->name);
    semantic_analysis_toplevel(top);
    codegen_strlits();
    codegen_toplevel(top);
  }

  return 0;
}
