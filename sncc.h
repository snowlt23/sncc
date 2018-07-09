#ifndef _SNCC_H_
#define _SNCC_H_

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

typedef struct {
  void** data;
  int cap;
  int len;
} vector;

typedef enum {
  TOKEN_ADD,
  TOKEN_SUB,
  TOKEN_MUL,
  TOKEN_DIV,
  TOKEN_INTLIT,
  TOKEN_LPAREN,
  TOKEN_RPAREN,
  TOKEN_LBRACKET,
  TOKEN_RBRACKET,
  TOKEN_IDENT
} tokenkind;

typedef struct {
  tokenkind kind;
  union {
    int intval;
    char* ident;
  };
} token;

typedef enum {
  AST_ADD,
  AST_SUB,
  AST_MUL,
  AST_DIV,
  AST_INTLIT
} astkind;

typedef struct _ast {
  astkind kind;
  union {
    struct {
      struct _ast* left;
      struct _ast* right;
    };
    int intval;
  };
} astree;

typedef struct {
  vector* tokens;
  int pos;
} tokenstream;

// utilities
#define error(...) fprintf(stderr, __VA_ARGS__)

// string.c
char* string_copy(char* s);

// vector.c
vector* new_vector_cap(int cap);
vector* new_vector();
void* vector_get(vector* v, int index);
void vector_set(vector* v, int index, void* elem);
void vector_push(vector* v, void* elem);

// lexer.c
vector* lexer();
char* token_to_kindstr(token* token);
char* token_to_str(token* token);

// parser.c
tokenstream* new_tokenstream(vector* tokens);
astree* expression(tokenstream* ts);
char* ast_to_kindstr(astree* ast);

// codegen.c
void codegen(astree* ast);
// emit
void emit_global(char* name);
void emit_label(char* label);
void emit_pop(char* s);
void emit_return();

#endif
