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

typedef struct {
  struct _typenode* typ;
  int offset;
} varinfo;

typedef struct {
  char* name;
  void* value;
} mappair;
typedef struct {
  vector* vector;
} map;

typedef enum {
  TOKEN_ADD,
  TOKEN_SUB,
  TOKEN_MUL,
  TOKEN_DIV,
  TOKEN_LESSER,
  TOKEN_GREATER,
  TOKEN_ASSIGN,
  TOKEN_AND,
  TOKEN_INTLIT,
  TOKEN_LPAREN,
  TOKEN_RPAREN,
  TOKEN_LBRACKET,
  TOKEN_RBRACKET,
  TOKEN_LBRACE,
  TOKEN_RBRACE,
  TOKEN_COMMA,
  TOKEN_SEMICOLON,
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
  AST_LESSER,
  AST_MINUS,
  AST_ASSIGN,
  AST_ADDR,
  AST_DEREF,
  AST_VARDECL,
  AST_INTLIT,
  AST_IDENT,
  AST_CALL,
  AST_STATEMENT,
  AST_FUNCDEF,
  AST_IF,
  AST_WHILE,
  AST_SIZEOF_EXPR,
  AST_SIZEOF_TYPE
} astkind;

typedef enum {
  TYPE_INT,
  TYPE_PTR,
  TYPE_ARRAY
} typekind;

typedef struct _typenode {
  typekind kind;
  struct _typenode* ptrof;
  size_t arraysize;
} typenode;

typedef struct {
  typenode* typ;
  char* name;
  int offset;
} paramtype;

typedef struct {
  paramtype* fdecl;
  vector* argdecls;
  vector* body;
  int stacksize;
} funcdecl;

typedef struct _astree {
  astkind kind;
  typenode* typ;
  union {
    struct {
      struct _astree* left;
      struct _astree* right;
    };
    paramtype* vardecl;
    struct _astree* value;
    typenode* typedesc;
    int intval;
    struct {
      char* ident;
      int offset;
    };
    struct {
      struct _astree* call;
      vector* arguments;
    };
    struct {
      struct _astree* ifcond;
      struct _astree* ifbody;
      struct _astree* elsebody;
    };
    struct {
      struct _astree* whilecond;
      struct _astree* whilebody;
    };
    vector* stmt;
  };
} astree;

typedef struct {
  vector* tokens;
  int pos;
} tokenstream;

// utilities
#define error(...) {fprintf(stderr, __VA_ARGS__); exit(1);}

// vector.c
vector* new_vector_cap(int cap);
vector* new_vector();
void* vector_get(vector* v, int index);
void vector_set(vector* v, int index, void* elem);
void vector_push(vector* v, void* elem);

// map.c
map* new_map_cap(int cap);
map* new_map();
void* map_get(map* m, char* name);
void map_insert(map* m, char* name, void* value);

// lexer.c
vector* lexer();
char* token_to_kindstr(token* token);
char* token_to_str(token* token);

// parser.c
// ast
char* ast_to_kindstr(astree* ast);
// type
typenode* new_typenode(typekind kind);
typenode* new_ptrnode(typenode* typ);
// tokenstream
tokenstream* new_tokenstream(vector* tokens);
token* get_token(tokenstream* ts);
void next_token(tokenstream* ts);
astree* expression(tokenstream* ts);
// statement
paramtype* parse_paramtype(tokenstream* ts);
vector* parse_statement(tokenstream* ts);
astree* parse_compound(tokenstream* ts);
// funcdecl
funcdecl parse_funcdecl(tokenstream* ts);

// semantic.c
int typesize(typenode* tn);
void semantic_analysis(astree* ast);
void semantic_analysis_funcdecl(funcdecl* fdecl);

// codegen.c
void codegen(astree* ast);
void codegen_funcdecl(funcdecl fdecl);
// emit
void emit_global(char* name);
void emit_label(char* label);
void emit_pop(char* s);
void emit_return();
void emit_prologue(int localsize);
void emit_epilogue();

#endif
