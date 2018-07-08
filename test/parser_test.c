#include "../vector.c"
#include "../lexer.c"
#include "../parser.c"

int main() {
  vector* tokenss = lexer();
  tokenstream* ts = new_tokenstream(tokenss);
  astree* ast = parser_top(ts);
  printf("%s", ast_to_kindstr(ast));
}
