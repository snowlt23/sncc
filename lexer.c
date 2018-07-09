#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>

typedef enum {
  TOKEN_ADD,
  TOKEN_SUB,
  TOKEN_MUL,
  TOKEN_DIV,
  TOKEN_INTLIT,
  TOKEN_LPAREN,
  TOKEN_RPAREN,
  TOKEN_LBRACKET,
  TOKEN_RBRACKET
} tokenkind;

typedef struct {
  tokenkind kind;
  union {
    int intval;
  };
} token;

token* new_token(tokenkind kind) {
  token* t = (token*)malloc(sizeof(token));
  t->kind = kind;
  return t;
}

token* new_intlit(int x) {
  token* t = new_token(TOKEN_INTLIT);
  t->intval = x;
  return t;
}

char* int_to_str(int x) {
  char* buf = (char*)malloc(6+1);
  snprintf(buf, 6+1, "%d", x);
  return buf;
}

char* token_to_kindstr(token* token) {
  if (token->kind == TOKEN_ADD) {
    return "TOKEN_ADD";
  } else if (token->kind == TOKEN_SUB) {
    return "TOKEN_SUB";
  } else if (token->kind == TOKEN_MUL) {
    return "TOKEN_MUL";
  } else if (token->kind == TOKEN_DIV) {
    return "TOKEN_DIV";
  } else if (token->kind == TOKEN_INTLIT) {
    return "TOKEN_INTLIT";
  } else if (token->kind == TOKEN_LPAREN) {
    return "TOKEN_LPAREN";
  } else if (token->kind == TOKEN_RPAREN) {
    return "TOKEN_RPAREN";
  } else if (token->kind == TOKEN_LBRACKET) {
    return "TOKEN_LBRACKET";
  } else if (token->kind == TOKEN_RBRACKET) {
    return "TOKEN_RBRACKET";
  } else {
    assert(0);
  }
}
char* token_to_str(token* token) {
  if (token->kind == TOKEN_ADD) {
    return "+";
  } else if (token->kind == TOKEN_SUB) {
    return "-";
  } else if (token->kind == TOKEN_MUL) {
    return "*";
  } else if (token->kind == TOKEN_DIV) {
    return "/";
  } else if (token->kind == TOKEN_LPAREN) {
    return "(";
  } else if (token->kind == TOKEN_RPAREN) {
    return ")";
  } else if (token->kind == TOKEN_LBRACKET) {
    return "{";
  } else if (token->kind == TOKEN_RBRACKET) {
    return "}";
  } else if (token->kind == TOKEN_INTLIT) {
    return int_to_str(token->intval);
  } else {
    assert(0);
  }
}

vector* lexer() {
  vector* tokenss = new_vector();

  for (;;) {
    char c = getc(stdin);
    if (c == EOF) {
      break;
    } else if (isdigit(c)) { // int literal
      char digitbuf[256] = {};
      digitbuf[0] = c;
      for (int i=1; ; i++) {
        assert(i < 256);
        char nc = getc(stdin);
        if (!isdigit(nc)) {
          ungetc(nc, stdin);
          vector_push(tokenss, new_intlit(atoi(digitbuf)));
          break;
        }
        digitbuf[i] = nc;
      }
    } else if (c == '+') {
      vector_push(tokenss, new_token(TOKEN_ADD));
    } else if (c == '-') {
      vector_push(tokenss, new_token(TOKEN_SUB));
    } else if (c == '*') {
      vector_push(tokenss, new_token(TOKEN_MUL));
    } else if (c == '/') {
      vector_push(tokenss, new_token(TOKEN_DIV));
    } else if (c == '(') {
      vector_push(tokenss, new_token(TOKEN_LPAREN));
    } else if (c == ')') {
      vector_push(tokenss, new_token(TOKEN_RPAREN));
    } else if (c == '{') {
      vector_push(tokenss, new_token(TOKEN_LBRACKET));
    } else if (c == '}') {
      vector_push(tokenss, new_token(TOKEN_RBRACKET));
    } else if (c == ' ') {
      continue;
    } else if (c == '\n') {
      continue;
    } else { // identifier
      error("unexpected token %c.", c);
    }
  }
  return tokenss;
}
