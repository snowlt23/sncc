#include <stdio.h>
#include "../sncc.h"

int main() {
  vector* tokenss = lexer();
  tokenstream* ts = new_tokenstream(tokenss);
  statement stmt = parse_statement(ts);
  printf("%d", statement_len(stmt));
  for (int i=0; i<statement_len(stmt); i++) {
    astree* ast = statement_get(stmt, i);
    printf(" %s", ast_to_kindstr(ast));
  }
}
