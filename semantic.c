#include <stdio.h>
#include <assert.h>
#include "sncc.h"

map* varmap;
int varpos;

varinfo* new_varinfo(typenode* typ, int offset) {
  varinfo* info = malloc(sizeof(varinfo));
  info->typ = typ;
  info->offset = offset;
  return info;
}

void init_semantic() {
  varmap = new_map();
  varpos = 0;
}

int typesize(typenode* typ) {
  if (typ->truetype != NULL) {
    return typesize(typ->truetype);
  }

  if (typ->kind == TYPE_INT) {
    return 4;
  } else if (typ->kind == TYPE_PTR) {
    return 8;
  } else if (typ->kind == TYPE_ARRAY) {
    return typesize(typ->ptrof) * typ->arraysize;
  } else {
    assert(false);
  }
}

void semantic_analysis(astree* ast) {
  if (ast->kind == AST_IDENT) {
    varinfo* info = map_get(varmap, ast->ident);
    if (info == NULL) error("undeclared %s variable.", ast->ident);
    ast->typ = info->typ;
    ast->offset = info->offset;
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
    if (ast->vardecl->typ->kind == TYPE_INT) {
      varpos += 8;
    } else {
      varpos += typesize(ast->vardecl->typ);
    }
    map_insert(varmap, ast->vardecl->name, new_varinfo(ast->vardecl->typ, varpos));
  } else if (ast->kind == AST_CALL && ast->call->kind == AST_IDENT) {
    for (int i=0; i<ast->arguments->len; i++) {
      semantic_analysis(vector_get(ast->arguments, i));
    }
    ast->typ = new_typenode(TYPE_INT); // FIXME:
  } else if (ast->kind == AST_STATEMENT) {
    for (int i=0; i<ast->stmt->len; i++) {
      astree* e = vector_get(ast->stmt, i);
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
  } else if (ast->kind == AST_SIZEOF_EXPR) {
    semantic_analysis(ast->value);
    ast->kind = AST_INTLIT;
    ast->intval = typesize(ast->value->typ);
    ast->typ = new_typenode(TYPE_INT);
  } else if (ast->kind == AST_SIZEOF_TYPE) {
    ast->kind = AST_INTLIT;
    ast->intval = typesize(ast->typedesc);
    ast->typ = new_typenode(TYPE_INT);
  } else {
    error("unsupported %d kind in codegen", ast->kind);
  }
}

void semantic_analysis_funcdecl(funcdecl* fdecl) {
  init_semantic();
  for (int i=0; i<fdecl->argdecls->len; i++) {
    paramtype* argparam = vector_get(fdecl->argdecls, i);
    varpos += 8;
    argparam->offset = varpos;
    map_insert(varmap, argparam->name, new_varinfo(argparam->typ, varpos));
  }
  for (int i=0; i<fdecl->body->len; i++) {
    semantic_analysis(vector_get(fdecl->body, i));
  }
  fdecl->stacksize = varpos;
}
