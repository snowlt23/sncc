#include <stdio.h>
#include "../sncc.h"

int main() {
  vector* tokenss = lexer();
  tokenstream* ts = new_tokenstream(tokenss);
  astree* ast = expression(ts);
  printf("%s", ast_to_kindstr(ast));
}
