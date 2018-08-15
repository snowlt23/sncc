#include <stdio.h>
#include "../sncc.h"

int main() {
  init_lexer();
  vector* tokenss = lexer();
  for (int i=0; i<tokenss->len; i++) {
    token* t = (token*)vector_get(tokenss, i);
    printf("%s:%s ", token_to_kindstr(t), token_to_str(t));
  }
}
