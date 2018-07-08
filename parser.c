#include <stdlib.h>

typedef enum {
  AST_ADD,
  AST_SUB,
  AST_INTLIT
} astkind;

typedef struct _ast {
  astkind kind;
  union {
    struct _ast* left;
    struct _ast* right;
    int intval;
  };
} astree;

typedef struct {
  vector* tokens;
  int pos;
} tokenstream;

astree* new_ast(astkind kind) {
  astree* ast = (astree*)malloc(sizeof(ast));
  ast->kind = kind;
  return ast;
}

astree* new_ast_infix(astkind kind, astree* left, astree* right) {
  astree* ast = new_ast(kind);
  ast->left = left;
  ast->right = right;
  return ast;
}

astree* new_ast_intlit(int x) {
  astree* ast = new_ast(AST_INTLIT);
  ast->intval = x;
  return ast;
}

char* ast_to_kindstr(astree* ast) {
  if (ast->kind == AST_ADD) {
    return "AST_ADD";
  } else if (ast->kind == AST_SUB) {
    return "AST_SUB";
  } else if (ast->kind == AST_INTLIT) {
    return "AST_INTLIT";
  } else {
    assert(0);
  }
}

tokenstream* new_tokenstream(vector* tokens) {
  tokenstream* ts = (tokenstream*)malloc(sizeof(tokenstream));
  ts->tokens = tokens;
  ts->pos = 0;
  return ts;
}
token* get_token(tokenstream* ts) {
  if (ts->pos >= ts->tokens->len) {
    return NULL;
  }
  return (token*)vector_get(ts->tokens, ts->pos);
}
void next_token(tokenstream* ts) {
  ts->pos++;
}

astree* parser_top(tokenstream* ts);

astree* factor(tokenstream* ts) {
  token* t = get_token(ts);
  if (t->kind == TOKEN_INTLIT) {
    next_token(ts);
    return new_ast_intlit(t->intval);
  } else {
    parser_top(ts);
  }
}

astree* infix_addsub(tokenstream* ts) {
  astree* left = factor(ts);
  for (;;) {
    token* t = get_token(ts);
    if (t == NULL) break;
    if (t->kind == TOKEN_ADD) {
      next_token(ts);
      astree* right = factor(ts);
      left = new_ast_infix(AST_ADD, left, right);
    } else if (t->kind == TOKEN_SUB) {
      next_token(ts);
      astree* right = factor(ts);
      left = new_ast_infix(AST_SUB, left, right);
    } else {
      break;
    }
  }
  return left;
}

astree* parser_top(tokenstream* ts) {
  return infix_addsub(ts);
}
