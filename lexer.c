#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <string.h>
#include <ctype.h>
#include "sncc.h"

FILE* input;
map* definemap;

void init_lexer() {
  input = stdin;
  definemap = new_map();
}

token* new_token(tokenkind kind) {
  token* t = malloc(sizeof(token));
  t->kind = kind;
  return t;
}

token* new_intlit(int x) {
  token* t = new_token(TOKEN_INTLIT);
  t->intval = x;
  return t;
}
token* new_strlit(char* s) {
  token* t = new_token(TOKEN_STRLIT);
  t->strval = s;
  return t;
}
token* new_ident(char* s) {
  token* t = new_token(TOKEN_IDENT);
  t->ident = strdup(s);
  return t;
}

char* int_to_str(int x) {
  char* buf = malloc(6+1);
  snprintf(buf, 6, "%d", x);
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
  } else if (token->kind == TOKEN_LESSER) {
    return "TOKEN_LESSER";
  } else if (token->kind == TOKEN_GREATER) {
    return "TOKEN_GREATER";
  } else if (token->kind == TOKEN_LESSEREQ) {
    return "TOKEN_LESSEREQ";
  } else if (token->kind == TOKEN_GREATEREQ) {
    return "TOKEN_GREATEREQ";
  } else if (token->kind == TOKEN_ASSIGN) {
    return "TOKEN_ASSIGN";
  } else if (token->kind == TOKEN_ADDASSIGN) {
    return "TOKEN_ADDASSIGN";
  } else if (token->kind == TOKEN_MULASSIGN) {
    return "TOKEN_MULASSIGN";
  } else if (token->kind == TOKEN_LAND) {
    return "TOKEN_LAND";
  } else if (token->kind == TOKEN_LOR) {
    return "TOKEN_LOR";
  } else if (token->kind == TOKEN_INC) {
    return "TOKEN_INC";
  } else if (token->kind == TOKEN_EQ) {
    return "TOKEN_EQ";
  } else if (token->kind == TOKEN_NOTEQ) {
    return "TOKEN_NOTEQ";
  } else if (token->kind == TOKEN_NOT) {
    return "TOKEN_NOT";
  } else if (token->kind == TOKEN_DOT) {
    return "TOKEN_DOT";
  } else if (token->kind == TOKEN_ALLOW) {
    return "TOKEN_ALLOW";
  } else if (token->kind == TOKEN_AND) {
    return "TOKEN_AND";
  } else if (token->kind == TOKEN_INTLIT) {
    return "TOKEN_INTLIT";
  } else if (token->kind == TOKEN_STRLIT) {
    return "TOKEN_STRLIT";
  } else if (token->kind == TOKEN_LPAREN) {
    return "TOKEN_LPAREN";
  } else if (token->kind == TOKEN_RPAREN) {
    return "TOKEN_RPAREN";
  } else if (token->kind == TOKEN_LBRACKET) {
    return "TOKEN_LBRACKET";
  } else if (token->kind == TOKEN_RBRACKET) {
    return "TOKEN_RBRACKET";
  } else if (token->kind == TOKEN_LBRACE) {
    return "TOKEN_LBRACE";
  } else if (token->kind == TOKEN_RBRACE) {
    return "TOKEN_RBRACE";
  } else if (token->kind == TOKEN_COMMA) {
    return "TOKEN_COMMA";
  } else if (token->kind == TOKEN_SEMICOLON) {
    return "TOKEN_SEMICOLON";
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
  } else if (token->kind == TOKEN_LESSER) {
    return "<";
  } else if (token->kind == TOKEN_GREATER) {
    return ">";
  } else if (token->kind == TOKEN_LESSEREQ) {
    return "<=";
  } else if (token->kind == TOKEN_GREATEREQ) {
    return ">=";
  } else if (token->kind == TOKEN_ASSIGN) {
    return "=";
  } else if (token->kind == TOKEN_ADDASSIGN) {
    return "+=";
  } else if (token->kind == TOKEN_MULASSIGN) {
    return "*=";
  } else if (token->kind == TOKEN_LAND) {
    return "&&";
  } else if (token->kind == TOKEN_LOR) {
    return "||";
  } else if (token->kind == TOKEN_INC) {
    return "++";
  } else if (token->kind == TOKEN_EQ) {
    return "==";
  } else if (token->kind == TOKEN_NOT) {
    return "!";
  } else if (token->kind == TOKEN_DOT) {
    return ".";
  } else if (token->kind == TOKEN_ALLOW) {
    return "->";
  } else if (token->kind == TOKEN_AND) {
    return "&";
  } else if (token->kind == TOKEN_LPAREN) {
    return "(";
  } else if (token->kind == TOKEN_RPAREN) {
    return ")";
  } else if (token->kind == TOKEN_LBRACE) {
    return "{";
  } else if (token->kind == TOKEN_RBRACE) {
    return "}";
  } else if (token->kind == TOKEN_LBRACKET) {
    return "[";
  } else if (token->kind == TOKEN_RBRACKET) {
    return "]";
  } else if (token->kind == TOKEN_COMMA) {
    return ",";
  } else if (token->kind == TOKEN_SEMICOLON) {
    return ";";
  } else if (token->kind == TOKEN_INTLIT) {
    return int_to_str(token->intval);
  } else if (token->kind == TOKEN_STRLIT) {
    return token->strval;
  } else if (token->kind == TOKEN_IDENT) {
    return token->ident;
  } else {
    assert(0);
  }
}

void skip_spaces() {
  while (true) {
    char nc = getc(input);
    if (nc != ' ') {
      ungetc(nc, input);
      break;
    }
  }
}

char* read_ident() {
  skip_spaces();
  char identbuf[256];
  int i = 0;
  while (true) {
    if (i >= 256) assert(false);
    char nc = getc(input);
    if (!isidenttail(nc)) {
      ungetc(nc, input);
      identbuf[i] = 0;
      break;
    }
    identbuf[i] = nc;
    i++;
  }
  return strdup(identbuf);
}

char* read_strlit() {
  char strbuf[1024];
  bool inesc = false;
  int i = 0;
  while (true) {
    assert(i < 1024);
    char nc = getc(input);
    if (inesc && nc == '\\') {
      strbuf[i] = '\\';
      strbuf[i+1] = '\\';
      inesc = false;
      i++;
    } else if (inesc) {
      strbuf[i] = '\\';
      strbuf[i+1] = nc;
      inesc = false;
      i++;
    } else if (nc == '\\') {
      inesc = true;
      i = i-1;
    } else if (nc == '"') {
      strbuf[i] = 0;
      break;
    } else if (nc == EOF) {
      error("expect end of string literal.");
    } else {
      strbuf[i] = nc;
    }
    i++;
  }
  return strdup(strbuf);
}

char* read_strlit_angled() {
  skip_spaces();
  char strbuf[1024];
  int i = 0;
  while (true) {
    assert(i < 1024);
    char nc = getc(input);
    if (nc == '>') {
      strbuf[i] = 0;
      break;
    }
    if (nc == EOF) error("expect end of string literal.");
    strbuf[i] = nc;
    i++;
  }
  return strdup(strbuf);
}

void preprocessor(vector* tokenss, char* prename) {
  if (strcmp(prename, "include") == 0) {
    skip_spaces();
    char c = getc(input);
    char* includename;
    if (c == '"') {
      includename = read_strlit();
    } else if (c == '<') {
      includename = malloc(sizeof(char)*1024+sizeof(char));
      char* slit = read_strlit_angled();
      snprintf(includename, 1024, "include/%s", slit);
    } else {
      error("#include expected string literal.");
    }
    FILE* tmpf = input;
    input = fopen(includename, "r");
    if (input == NULL) { fprintf(stderr, "%s: No such file", includename);
      exit(1);
    }
    vector* includetokens = lexer();
    for (int i=0; i<includetokens->len; i++) {
      vector_push(tokenss, vector_get(includetokens, i));
    }
    fclose(input);
    input = tmpf;
  } else if (strcmp(prename, "define") == 0) {
    char* definename = read_ident();
    vector* definetokens = new_vector();
    while (true) {
      skip_spaces();
      char c = getc(input);
      if (c == '\n') break;
      ungetc(c, input);
      bool cont = single_token_lexer(definetokens);
      if (!cont) break;
    }
    map_insert(definemap, definename, definetokens);
  } else {
    error("unsupported preprocessor.");
  }
}

bool single_token_lexer(vector* tokenss) {
  char c = getc(input);

  if (c == EOF) {
    return false;
  } else if (isdigit(c)) { // int literal
    char digitbuf[256];
    digitbuf[0] = c;
    int i=1;
    while (true) {
      assert(i < 256);
      char nc = getc(input);
      if (!isdigit(nc)) {
        ungetc(nc, input);
        digitbuf[i] = 0;
        vector_push(tokenss, new_intlit(atoi(digitbuf)));
        break;
      }
      digitbuf[i] = nc;
      i++;
    }
  } else if (c == '#') { // preprocessor
    char* prename = read_ident();
    preprocessor(tokenss, prename);
  } else if (c == '"') { // string literal
    vector_push(tokenss, new_strlit(read_strlit()));
  } else if (c == '\'') {
    char nc = getc(input);
    if (nc == '\\') {
      nc = getc(input);
      if (nc == '0') {
        nc = '\0';
      }
      if (nc == 'n') {
        nc = '\n';
      }
    }
    if (getc(input) != '\'') {
      error("expect end of char literal.");
    }
    vector_push(tokenss, new_intlit(nc));
  } else if (c == '+') {
    char nc = getc(input);
    if (nc == '+') {
      vector_push(tokenss, new_token(TOKEN_INC));
    } else if (nc == '=') {
      vector_push(tokenss, new_token(TOKEN_ADDASSIGN));
    } else {
      ungetc(nc, input);
      vector_push(tokenss, new_token(TOKEN_ADD));
    }
  } else if (c == '-') {
    char nc = getc(input);
    if (nc == '>') {
      vector_push(tokenss, new_token(TOKEN_ALLOW));
    } else {
      ungetc(nc, input);
      vector_push(tokenss, new_token(TOKEN_SUB));
    }
  } else if (c == '*') {
    char nc = getc(input);
    if (nc == '=') {
      vector_push(tokenss, new_token(TOKEN_MULASSIGN));
    } else {
      ungetc(nc, input);
      vector_push(tokenss, new_token(TOKEN_MUL));
    }
  } else if (c == '/') {
    char nc = getc(input);
    if (nc == '/') {
      while (true) {
        nc = getc(input);
        if (nc == EOF || nc == '\n') break;
      }
    } else if (nc == '*') {
      bool inasterisk = false;
      while (true) {
        nc = getc(input);
        if (inasterisk && nc == '/') {
          break;
        }
        if (nc == '*') {
          inasterisk = true;
          continue;
        }
        if (nc == EOF) break;
        inasterisk = false;
      }
    } else {
      ungetc(nc, input);
      vector_push(tokenss, new_token(TOKEN_DIV));
    }
  } else if (c == '%') {
    vector_push(tokenss, new_token(TOKEN_MOD));
  } else if (c == '<') {
    char nc = getc(input);
    if (nc == '=') {
      vector_push(tokenss, new_token(TOKEN_LESSEREQ));
    } else {
      ungetc(nc, input);
      vector_push(tokenss, new_token(TOKEN_LESSER));
    }
  } else if (c == '>') {
    char nc = getc(input);
    if (nc == '=') {
      vector_push(tokenss, new_token(TOKEN_GREATEREQ));
    } else {
      ungetc(nc, input);
      vector_push(tokenss, new_token(TOKEN_GREATER));
    }
  } else if (c == '.') {
    vector_push(tokenss, new_token(TOKEN_DOT));
  } else if (c == '|') {
    char nc = getc(input);
    if (nc == '|') {
      vector_push(tokenss, new_token(TOKEN_LOR));
    } else {
      error("unsupported | token in currently.");
    }
  } else if (c == '&') {
    char nc = getc(input);
    if (nc == '&') {
      vector_push(tokenss, new_token(TOKEN_LAND));
    } else {
      ungetc(nc, input);
      vector_push(tokenss, new_token(TOKEN_AND));
    }
  } else if (c == '=') {
    char nc = getc(input);
    if (nc == '=') {
      vector_push(tokenss, new_token(TOKEN_EQ));
    } else {
      ungetc(nc, input);
      vector_push(tokenss, new_token(TOKEN_ASSIGN));
    }
  } else if (c == '!') {
    char nc = getc(input);
    if (nc == '=') {
      vector_push(tokenss, new_token(TOKEN_NOTEQ));
    } else {
      ungetc(nc, input);
      vector_push(tokenss, new_token(TOKEN_NOT));
    }
  } else if (c == '(') {
    vector_push(tokenss, new_token(TOKEN_LPAREN));
  } else if (c == ')') {
    vector_push(tokenss, new_token(TOKEN_RPAREN));
  } else if (c == '[') {
    vector_push(tokenss, new_token(TOKEN_LBRACKET));
  } else if (c == ']') {
    vector_push(tokenss, new_token(TOKEN_RBRACKET));
  } else if (c == '{') {
    vector_push(tokenss, new_token(TOKEN_LBRACE));
  } else if (c == '}') {
    vector_push(tokenss, new_token(TOKEN_RBRACE));
  } else if (c == ',') {
    vector_push(tokenss, new_token(TOKEN_COMMA));
  } else if (c == ';') {
    vector_push(tokenss, new_token(TOKEN_SEMICOLON));
  } else if (c == ' ') {
    return true;
  } else if (c == '\n') {
    return true;
  } else if (isident(c)) {
    ungetc(c, input);
    char* id = read_ident();
    vector* definetokens = map_get(definemap, id);
    if (definetokens != NULL) {
      for (int i=0; i<definetokens->len; i++) {
        vector_push(tokenss, vector_get(definetokens, i));
      }
    } else {
      vector_push(tokenss, new_ident(id));
    }
  } else {
    fprintf(stderr, "%d", c);
    fprintf(stderr, "unexpected token '%c'\n", c);
    exit(1);
  }
  return true;
}

vector* lexer() {
  vector* tokenss = new_vector();

  while (true) {
    bool cont = single_token_lexer(tokenss);
    if (!cont) break;
  }
  return tokenss;
}
