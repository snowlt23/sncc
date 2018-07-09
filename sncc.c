#include <stdio.h>
#include "sncc.h"

int main() {
  vector* tokenss = lexer();
  tokenstream* ts = new_tokenstream(tokenss);
  astree* ast = expression(ts);

  emit_global("main");
  emit_label("main");
  codegen(ast);
  emit_pop("%eax");
  emit_return();
  return 0;
}
