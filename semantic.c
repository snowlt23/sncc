#include <stdio.h>
#include <assert.h>
#include "sncc.h"

void semantic_analysis(map* varmap, astree* ast) {
  if (ast->kind == AST_IDENT) {
    mapelem elem = map_get(varmap, ast->ident);
    if (elem.pos == -1) error("undeclared %s variable.", ast->ident);
    ast->typ = elem.typ;
  } else if (ast->kind == AST_INTLIT) {
    ast->typ = new_typenode(TYPE_INT);
  } else if (ast->kind == AST_ADD || ast->kind == AST_SUB) {
    semantic_analysis(varmap, ast->left);
    semantic_analysis(varmap, ast->right);
    if (ast->left->typ->kind == TYPE_PTR && ast->right->typ->kind == TYPE_INT) {
      ast->typ = ast->left->typ;
    } else if (ast->left->typ->kind == TYPE_INT && ast->right->typ->kind == TYPE_INT) {
      ast->typ = new_typenode(TYPE_INT);;
    } else {
      error("illegal add arithmetic.");
    }
  } else if (ast->kind == AST_MUL || ast->kind == AST_DIV || ast->kind == AST_LESSER) {
    semantic_analysis(varmap, ast->left);
    semantic_analysis(varmap, ast->right);
    if (ast->left->typ->kind == TYPE_INT && ast->right->typ->kind == TYPE_INT) {
      ast->typ = new_typenode(TYPE_INT);
    } else {
      error("illegal add arithmetic.");
    }
  } else if (ast->kind == AST_MINUS) {
    semantic_analysis(varmap, ast->value);
    ast->typ = ast->value->typ;
  } else if (ast->kind == AST_ASSIGN) {
    semantic_analysis(varmap, ast->left);
    semantic_analysis(varmap, ast->right);
    if (ast->left->typ->kind != ast->right->typ->kind) {
      error("type mismatch in assign.");
    }
    ast->typ = new_typenode(TYPE_INT);
  } else if (ast->kind == AST_ADDR) {
    semantic_analysis(varmap, ast->value);
    ast->typ = new_ptrnode(ast->value->typ);
  } else if (ast->kind == AST_DEREF) {
    semantic_analysis(varmap, ast->value);
    if (ast->value->typ->kind != TYPE_PTR) error("cannot deref operator apply to noptr.");
    ast->typ = ast->value->typ->ptrof;
  } else if (ast->kind == AST_VARDECL) {
    mapelem elem;
    elem.typ = ast->vardecl->typ;
    map_insert(varmap, ast->vardecl->name, elem);
  } else if (ast->kind == AST_CALL && ast->call->kind == AST_IDENT) {
    for (int i=0; i<ast->arguments->len; i++) {
      semantic_analysis(varmap, vector_get(ast->arguments, i));
    }
    ast->typ = new_typenode(TYPE_INT); // FIXME:
  } else if (ast->kind == AST_STATEMENT) {
    for (int i=0; i<statement_len(ast->stmt); i++) {
      astree* e = statement_get(ast->stmt, i);
      semantic_analysis(varmap, e);
    }
  } else if (ast->kind == AST_IF) {
    semantic_analysis(varmap, ast->ifcond);
    semantic_analysis(varmap, ast->ifbody);
    if (ast->elsebody != NULL) {
      semantic_analysis(varmap, ast->elsebody);
    }
  } else if (ast->kind == AST_WHILE) {
    semantic_analysis(varmap, ast->whilecond);
    semantic_analysis(varmap, ast->whilebody);
  } else {
    error("unsupported %d kind in codegen", ast->kind);
  }
}

void semantic_analysis_funcdecl(funcdecl fdecl) {
  map* varmap = new_map();
  for (int i=0; i<paramtypelist_len(fdecl.argdecls); i++) {
    paramtype* argparam = paramtypelist_get(fdecl.argdecls, i);
    mapelem elem;
    elem.typ = argparam->typ;
    map_insert(varmap, argparam->name, elem);
  }
  for (int i=0; i<statement_len(fdecl.body); i++) {
    semantic_analysis(varmap, statement_get(fdecl.body, i));
  }
}
