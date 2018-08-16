#include <stdio.h>
#include "sncc.self.h"

int main() {
  init_lexer();
  init_parser();
  vector* tokenss = lexer();
  tokenstream* ts = new_tokenstream(tokenss);

  init_semantic();
  while (true) {
    if (get_token(ts) == NULL) break;
    toplevel* top = parse_toplevel(ts);
    semantic_analysis_toplevel(top);
    codegen_strlits();
    codegen_toplevel(top);
  }

  return 0;
}
