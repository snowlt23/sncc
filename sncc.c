#include <stdio.h>
#include "sncc.h"

int main() {
  vector* tokenss = lexer();
  tokenstream* ts = new_tokenstream(tokenss);
  statement stmt = parse_statement(ts);

  emit_global("main");
  emit_label("main");
  emit_prologue(0); // FIXME: localsize of prologue
  for (int i=0; i<statement_len(stmt); i++) {
    codegen(statement_get(stmt, i));
  }
  emit_pop("%eax");
  emit_epilogue();
  emit_return();
  return 0;
}
