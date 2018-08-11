#include <stdio.h>
#include <assert.h>
#include "sncc.h"

map* varmap;
int varpos;

void init_semantic() {
  varmap = new_map();
  varpos = 0;
}

void semantic_analysis(astree* ast) {
  if (ast->kind == AST_IDENT) {
    mapelem elem = map_get(varmap, ast->ident);
    if (elem.pos == -1) error("undeclared %s variable.", ast->ident);
    ast->typ = elem.typ;
    ast->offset = elem.pos;
  } else if (ast->kind == AST_INTLIT) {
    ast->typ = new_typenode(TYPE_INT);
  } else if (ast->kind == AST_ADD || ast->kind == AST_SUB) {
    semantic_analysis(ast->left);
    semantic_analysis(ast->right);
    if (ast->left->typ->kind == TYPE_PTR && ast->right->typ->kind == TYPE_INT) {
      ast->typ = ast->left->typ;
    } else if (ast->left->typ->kind == TYPE_INT && ast->right->typ->kind == TYPE_INT) {
      ast->typ = new_typenode(TYPE_INT);;
    } else {
      error("illegal add arithmetic.");
    }
  } else if (ast->kind == AST_MUL || ast->kind == AST_DIV || ast->kind == AST_LESSER) {
    semantic_analysis(ast->left);
    semantic_analysis(ast->right);
    if (ast->left->typ->kind == TYPE_INT && ast->right->typ->kind == TYPE_INT) {
      ast->typ = new_typenode(TYPE_INT);
    } else {
      error("illegal add arithmetic.");
    }
  } else if (ast->kind == AST_MINUS) {
    semantic_analysis(ast->value);
    ast->typ = ast->value->typ;
  } else if (ast->kind == AST_ASSIGN) {
    semantic_analysis(ast->left);
    semantic_analysis(ast->right);
    if (ast->left->typ->kind != ast->right->typ->kind) {
      error("type mismatch in assign.");
    }
    ast->typ = new_typenode(TYPE_INT);
  } else if (ast->kind == AST_ADDR) {
    semantic_analysis(ast->value);
    ast->typ = new_ptrnode(ast->value->typ);
  } else if (ast->kind == AST_DEREF) {
    semantic_analysis(ast->value);
    if (ast->value->typ->kind != TYPE_PTR) error("cannot deref operator apply to noptr.");
    ast->typ = ast->value->typ->ptrof;
  } else if (ast->kind == AST_VARDECL) {
    varpos += 8;
    mapelem elem;
    elem.typ = ast->vardecl->typ;
    elem.pos = varpos;
    map_insert(varmap, ast->vardecl->name, elem);
  } else if (ast->kind == AST_CALL && ast->call->kind == AST_IDENT) {
    for (int i=0; i<ast->arguments->len; i++) {
      semantic_analysis(vector_get(ast->arguments, i));
    }
    ast->typ = new_typenode(TYPE_INT); // FIXME:
  } else if (ast->kind == AST_STATEMENT) {
    for (int i=0; i<statement_len(ast->stmt); i++) {
      astree* e = statement_get(ast->stmt, i);
      semantic_analysis(e);
    }
  } else if (ast->kind == AST_IF) {
    semantic_analysis(ast->ifcond);
    semantic_analysis(ast->ifbody);
    if (ast->elsebody != NULL) {
      semantic_analysis(ast->elsebody);
    }
  } else if (ast->kind == AST_WHILE) {
    semantic_analysis(ast->whilecond);
    semantic_analysis(ast->whilebody);
  } else {
    error("unsupported %d kind in codegen", ast->kind);
  }
}

void semantic_analysis_funcdecl(funcdecl* fdecl) {
  init_semantic();
  for (int i=0; i<paramtypelist_len(fdecl->argdecls); i++) {
    paramtype* argparam = paramtypelist_get(fdecl->argdecls, i);
    varpos += 8;
    mapelem elem;
    elem.typ = argparam->typ;
    elem.pos = varpos;
    argparam->offset = varpos;
    map_insert(varmap, argparam->name, elem);
  }
  for (int i=0; i<statement_len(fdecl->body); i++) {
    semantic_analysis(statement_get(fdecl->body, i));
  }
  fdecl->stacksize = varpos;
}
