
#include <stdio.h>
#include "vector.c"
#include "lexer.c"
#include "parser.c"
#include "codegen.c"

int main() {
  vector* tokenss = lexer();
  tokenstream* ts = new_tokenstream(tokenss);
  astree* ast = parser_top(ts);

  emit_global("main");
  emit_label("main");
  codegen(ast);
  emit_pop("%rax");
  emit_return();
  return 0;
}
