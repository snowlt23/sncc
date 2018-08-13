#include <stdio.h>
#include "../sncc.h"

int main() {
  vector* tokenss = lexer();
  tokenstream* ts = new_tokenstream(tokenss);
  vector* stmt = parse_statements(ts);
  printf("%d", stmt->len);
  for (int i=0; i<stmt->len; i++) {
    astree* ast = vector_get(stmt, i);
    printf(" %s", ast_to_kindstr(ast));
  }
}
