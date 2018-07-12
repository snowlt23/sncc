#include <stdio.h>
#include "sncc.h"

void assign_variable_position(map* varmap, int* pos, astree* ast) {
  if (ast->kind == AST_ASSIGN) {
    if (ast->left->kind != AST_IDENT) error("current assign supports only variable.");
    *pos += 8;
    map_insert(varmap, ast->left->ident, *pos);
  }
}

int main() {
  vector* tokenss = lexer();
  tokenstream* ts = new_tokenstream(tokenss);
  statement stmt = parse_statement(ts);

  map* varmap = new_map();
  int varpos = 0;
  for (int i=0; i<statement_len(stmt); i++) {
    assign_variable_position(varmap, &varpos, statement_get(stmt, i));
  }

  emit_global("main");
  emit_label("main");
  emit_prologue(varpos);
  for (int i=0; i<statement_len(stmt); i++) {
    codegen(varmap, statement_get(stmt, i));
  }
  emit_pop("%rax");
  emit_epilogue();
  emit_return();
  return 0;
}
