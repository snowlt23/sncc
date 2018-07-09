#include <stdlib.h>
#include <assert.h>
#include "sncc.h"

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
  } else if (ast->kind == AST_MUL) {
    return "AST_MUL";
  } else if (ast->kind == AST_DIV) {
    return "AST_DIV";
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
  if (t == NULL) error("require more token");

  if (t->kind == TOKEN_LPAREN) {
    next_token(ts);
    astree* ret = expression(ts);
    token* t = get_token(ts);
    if (t == NULL || t->kind != TOKEN_RPAREN) error("\"(\" should be ended by \")\".");
    next_token(ts);
    return ret;
  } else if (t->kind == TOKEN_INTLIT) {
    next_token(ts);
    return new_ast_intlit(t->intval);
  } else {
    return expression(ts);
  }
}

astree* infix_muldiv(tokenstream* ts) {
  astree* left = factor(ts);
  for (;;) {
    token* t = get_token(ts);
    if (t == NULL) break;
    if (t->kind == TOKEN_MUL) {
      next_token(ts);
      astree* right = factor(ts);
      left = new_ast_infix(AST_MUL, left, right);
    } else if (t->kind == TOKEN_DIV) {
      next_token(ts);
      astree* right = factor(ts);
      left = new_ast_infix(AST_DIV, left, right);
    } else {
      break;
    }
  }
  return left;
}

astree* infix_addsub(tokenstream* ts) {
  astree* left = infix_muldiv(ts);
  for (;;) {
    token* t = get_token(ts);
    if (t == NULL) break;
    if (t->kind == TOKEN_ADD) {
      next_token(ts);
      astree* right = infix_muldiv(ts);
      left = new_ast_infix(AST_ADD, left, right);
    } else if (t->kind == TOKEN_SUB) {
      next_token(ts);
      astree* right = infix_muldiv(ts);
      left = new_ast_infix(AST_SUB, left, right);
    } else {
      break;
    }
  }
  return left;
}

astree* expression(tokenstream* ts) {
  return infix_addsub(ts);
}

typenode parse_typenode(tokenstream* ts) {
  token* t = get_token(ts); next_token(ts);
  if (t == NULL || t->kind != TOKEN_IDENT) error("expected type, but got %s.", token_to_str(t));
  typenode tnode;
  tnode.name = t->ident;
  return tnode;
}

declnode parse_declnode(tokenstream* ts) {
  token* t = get_token(ts); next_token(ts);
  if (t == NULL || t->kind != TOKEN_IDENT) error("expected declaration, but got %s", token_to_str(t));
  declnode dnode;
  dnode.name = t->ident;
  return dnode;
}

paramtype* parse_paramtype(tokenstream* ts) {
  paramtype* pt = (paramtype*)malloc(sizeof(paramtype));
  pt->type = parse_typenode(ts);
  pt->decl = parse_declnode(ts);
  return pt;
}

paramtypelist parse_paramtype_list(tokenstream* ts) {
  vector* ptlist = new_vector();
  for (;;) {
    if (get_token(ts) != NULL && get_token(ts)->kind != TOKEN_IDENT) break;
    paramtype* pt = parse_paramtype(ts);
    vector_push(ptlist, (void*)pt);
    token* t = get_token(ts);
    if (t == NULL) error("require more token.");
    if (t->kind != TOKEN_COMMA) break;
    next_token(ts);
  }
  paramtypelist ptl;
  ptl.vector = ptlist;
  return ptl;
}

paramtype paramtypelist_get(paramtypelist ptlist, int index) {
  return *(paramtype*)vector_get(ptlist.vector, index);
}

int paramtypelist_len(paramtypelist ptlist) {
  return ptlist.vector->len;
}

funcdecl parse_funcdecl(tokenstream* ts) {
  funcdecl fdecl;
  fdecl.returntype = parse_typenode(ts);
  fdecl.fdecl = parse_declnode(ts);
  token* lparen = get_token(ts); next_token(ts);
  if (lparen == NULL || lparen->kind != TOKEN_LPAREN) error("function decl expect \"(\", but got %s", token_to_str(lparen));
  fdecl.argdecls = parse_paramtype_list(ts);
  token* rparen = get_token(ts); next_token(ts);
  if (rparen == NULL || rparen->kind != TOKEN_RPAREN) error("function decl expect \")\", but got %s", token_to_str(rparen));
  return fdecl;
}
