#include <stdio.h>
#include "sncc.h"

int main() {
  init_parser();
  vector* tokenss = lexer();
  tokenstream* ts = new_tokenstream(tokenss);

  init_semantic();
  emit_global("main");
  for (;;) {
    if (get_token(ts) == NULL) break;
    toplevel top = parse_toplevel(ts);
    semantic_analysis_toplevel(&top);
    codegen_strlits();
    codegen_toplevel(top);
  }

  return 0;
}
