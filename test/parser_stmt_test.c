#include <stdio.h>
#include "../sncc.h"

int main() {
  init_lexer();
  init_parser();
  vector* tokenss = lexer();
  tokenstream* ts = new_tokenstream(tokenss);
  vector* stmts = parse_statements(ts);
  printf("%d", stmts->len);
  for (int i=0; i<stmts->len; i++) {
    astree* ast = vector_get(stmts, i);
    printf(" %s", ast_to_kindstr(ast));
  }
}
