#include <stdio.h>
#include <stdlib.h>
#include <ctype.h>
#include <assert.h>
#include "sncc.h"

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
token* new_ident(char* s) {
  token* t = new_token(TOKEN_IDENT);
  t->ident = string_copy(s);
}

char* int_to_str(int x) {
  char* buf = (char*)malloc(6+1);
  snprintf(buf, 6+1, "%d", x);
  return buf;
}
bool isident(char c) {
  return isalpha(c) || c == '_';
}
bool isidenttail(char c) {
  return isident(c) || isdigit(c);
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
  } else if (token->kind == TOKEN_ASSIGN) {
    return "TOKEN_ASSIGN";
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
  } else if (token->kind == TOKEN_COMMA) {
    return "TOKEN_COMMA";
  } else if (token->kind == TOKEN_IDENT) {
    return "TOKEN_IDENT";
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
  } else if (token->kind == TOKEN_ASSIGN) {
    return "=";
  } else if (token->kind == TOKEN_LPAREN) {
    return "(";
  } else if (token->kind == TOKEN_RPAREN) {
    return ")";
  } else if (token->kind == TOKEN_LBRACKET) {
    return "{";
  } else if (token->kind == TOKEN_RBRACKET) {
    return "}";
  } else if (token->kind == TOKEN_COMMA) {
    return ",";
  } else if (token->kind == TOKEN_INTLIT) {
    return int_to_str(token->intval);
  } else if (token->kind == TOKEN_IDENT) {
    return token->ident;
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
    } else if (c == '=') {
      vector_push(tokenss, new_token(TOKEN_ASSIGN));
    } else if (c == '(') {
      vector_push(tokenss, new_token(TOKEN_LPAREN));
    } else if (c == ')') {
      vector_push(tokenss, new_token(TOKEN_RPAREN));
    } else if (c == '{') {
      vector_push(tokenss, new_token(TOKEN_LBRACKET));
    } else if (c == '}') {
      vector_push(tokenss, new_token(TOKEN_RBRACKET));
    } else if (c == ',') {
      vector_push(tokenss, new_token(TOKEN_COMMA));
    } else if (c == ' ') {
      continue;
    } else if (c == '\n') {
      continue;
    } else if (isident(c)) { // identifier
      char identbuf[256] = {};
      identbuf[0] = c;
      for (int i=1; ; i++) {
        if (i >= 256) error("long length (>256) identifer is unsupported in currently."); // FIXME: long length identifer support.
        char nc = getc(stdin);
        if (!isidenttail(nc)) {
          ungetc(nc, stdin);
          vector_push(tokenss, new_ident(identbuf));
          break;
        }
        identbuf[i] = nc;
      }
    } else {
      error("unexpected token %c.", c);
    }
  }
  return tokenss;
}
